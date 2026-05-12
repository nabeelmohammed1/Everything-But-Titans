// Fill out your copyright notice in the Description page of Project Settings.


#include "AirShooterCharacter.h"
#include "AirAttributes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h" 

// Sets default values
AAirShooterCharacter::AAirShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // ensure the actor itself replicates

	// Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(ASCReplicationMode);

	// Capsule size
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Use movement-based rotation, not controller yaw
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character movement tuning (your values kept)
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 540.f, 0.f);
	Move->JumpZVelocity = 600.f;
	Move->AirControl = 0.2f;
	Move->MaxWalkSpeed = 600.f;
	Move->MinAnalogWalkSpeed = 150.f;
	Move->BrakingDecelerationWalking = 2048.f;
	Move->BrakingDecelerationFalling = 1500.f;

	//Add Air Attribute set
	AirAttributes = CreateDefaultSubobject<UAirAttributes>(TEXT("AirAttributes"));

	// Nameplate Widget
	// Attach above the character's head. The exact Z offset (120 cm) can be
	// tweaked in the Blueprint child class after construction.
	NameplateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidgetComponent"));
	NameplateWidgetComponent->SetupAttachment(GetRootComponent());
	NameplateWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	NameplateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // always faces camera
	NameplateWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));
	NameplateWidgetComponent->SetVisibility(false); // hidden until proven visible
}

// Called when the game starts or when spawned
void AAirShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent) 
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

		// NOTE: Nameplate timers are intentionally NOT started here anymore.
	//
	// The root cause of the host-can't-see-nameplates bug was that BeginPlay
	// fires before the pawn is fully possessed. On a listen server the host's
	// GetController() can still be null at BeginPlay time, so the very first
	// UpdateNameplateVisibility call bails out immediately (LocalPC == null)
	// and the widget is never shown.
	//
	// Timers are now started in:
	//   - PossessedBy()     → covers the listen-server host (and solo play)
	//   - OnRep_PlayerState → covers all remote clients
	// Both of those callbacks are guaranteed to fire after the controller
	// is fully set up, so GetController() is always valid when the first
	// timer tick runs.
}

// ============================================================================
//  Nameplate – Timer Startup
// ============================================================================

void AAirShooterCharacter::StartNameplateTimers()
{
	// Guard: only the local client (or listen-server host acting as a client)
	// should run these timers. Dedicated server has no viewport to check against.
	// Also guard against double-registration if both PossessedBy and
	// OnRep_PlayerState fire on the same actor (can happen in edge cases).
	if (!IsLocallyControlled() || bNameplateTimerStarted) return;
	if (GetNetMode() == NM_DedicatedServer) return;

	bNameplateTimerStarted = true;

	// Stagger start time slightly so all players don't fire at the same frame.
	const float RandomOffset = FMath::RandRange(0.f, NameplateCheckInterval);

	// Fast timer – drives the actual show/hide logic.
	GetWorldTimerManager().SetTimer(
		NameplateCheckTimerHandle,
		this,
		&AAirShooterCharacter::UpdateNameplateVisibility,
		NameplateCheckInterval,
		true,          // looping
		RandomOffset   // initial delay
	);

	// Slow timer – rebuilds the cached player list every 5 seconds.
	// This avoids iterating all world actors on every visibility check.
	GetWorldTimerManager().SetTimer(
		PlayerCacheRefreshTimerHandle,
		this,
		&AAirShooterCharacter::RefreshPlayerCache,
		5.f,
		true,
		0.f // run immediately so cache is populated before first check
	);
}

// ============================================================================
//  Nameplate — Cache Refresh  (runs every 5 s, local client only)
// ============================================================================

void AAirShooterCharacter::RefreshPlayerCache()
{
	CachedOtherPlayers.Empty();
	for (TActorIterator<AAirShooterCharacter> It(GetWorld()); It; ++It)
	{
		AAirShooterCharacter* Other = *It;
		if (Other && Other != this)
		{
			CachedOtherPlayers.Add(Other);
		}
	}
}

// ============================================================================
//  Nameplate — Visibility Update  (runs every NameplateCheckInterval, local client only)
// ============================================================================

void AAirShooterCharacter::UpdateNameplateVisibility()
{
	if (!IsLocallyControlled()) return;

	// Use GetController() cast to APlayerController rather than
	// GetFirstPlayerController(). GetFirstPlayerController() always returns
	// the host's controller, which breaks visibility for every client on a
	// listen server. GetController() always returns THIS pawn's own controller.
	//
	// Because timers now start from PossessedBy/OnRep_PlayerState (not
	// BeginPlay), GetController() is guaranteed to be valid here. The null
	// check below is just a safety net for unexpected edge cases.
	APlayerController* LocalPC = Cast<APlayerController>(GetController());
	if (!LocalPC)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Nameplate] %s: LocalPC is null in UpdateNameplateVisibility – skipping. "
				"This should not happen since timers start post-possession."),
			*GetName());
		return;
	}

	FVector  CameraLocation;
	FRotator CameraRotation;
	LocalPC->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const FVector CameraForward = CameraRotation.Vector();

	const float MaxDistSq = NameplateMaxDistance * NameplateMaxDistance;

	for (AAirShooterCharacter* Other : CachedOtherPlayers)
	{
		if (!IsValid(Other)) continue;

		// ------------------------------------------------------------------
		// Check 1 - Distance (squared, no sqrt cost)
		// ------------------------------------------------------------------
		const FVector ToOther = Other->GetActorLocation() - CameraLocation;
		const float   DistSq = ToOther.SizeSquared();

		if (DistSq > MaxDistSq)
		{
			Other->NameplateWidgetComponent->SetVisibility(false);
			continue;
		}

		// ------------------------------------------------------------------
		// Check 2 - FOV (dot product, no trig cost)
		// ------------------------------------------------------------------
		const FVector ToOtherNormal = ToOther.GetSafeNormal();
		const bool    bInFront = FVector::DotProduct(CameraForward, ToOtherNormal) > 0.f;

		if (!bInFront)
		{
			Other->NameplateWidgetComponent->SetVisibility(false);
			continue;
		}

		// ------------------------------------------------------------------
		// Check 3 - Line trace for wall occlusion
		//
		// We trace from the local camera to the other player's head position.
		// If anything blocks the trace, the nameplate is hidden.
		// Both pawns are ignored so the trace doesn't self-hit.
		// ------------------------------------------------------------------
		const FVector TraceEnd = Other->GetActorLocation() + FVector(0.f, 0.f, 60.f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Other);

		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			Hit,
			CameraLocation,
			TraceEnd,
			ECC_Visibility,
			Params
		);

		Other->NameplateWidgetComponent->SetVisibility(!bBlocked);
	}
}

// Called every frame
void AAirShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAirShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(AbilitySystemComponent && PlayerInputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm", // Confirm target
			"Cancel",  // Cancel target
			"EAirAbilityInputID", // Enum name
			static_cast<int32>(EAirAbilityInputID::Confirm), // Enum value for "unbound"
			static_cast<int32>(EAirAbilityInputID::Cancel) // Enum value for "movement abilities"
		);

		AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, Binds);
	}

}

void AAirShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
		GrantStartingAbilities();
	}

	// Start nameplate timers here for the listen-server host (and solo play).
	// PossessedBy fires server-side only, and on a listen server the host is
	// both server and local client, so IsLocallyControlled() is true here for
	// the host's own pawn. The controller is fully valid at this point.
	StartNameplateTimers();
}

void AAirShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// Start nameplate timers here for the listen-server host (and solo play).
	// PossessedBy fires server-side only, and on a listen server the host is
	// both server and local client, so IsLocallyControlled() is true here for
	// the host's own pawn. The controller is fully valid at this point.
	StartNameplateTimers();
}

UAbilitySystemComponent* AAirShooterCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AAirShooterCharacter::GrantStartingAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (const FStartingAbilityInfo& AbilityInfo : StartingAbilities)
	{
		if (AbilityInfo.AbilityClass)
		{
			GrantAbilityWithInput(AbilityInfo.AbilityClass, AbilityInfo.InputID);
		}
	}
}

void AAirShooterCharacter::GrantAbilityWithInput(TSubclassOf<UGameplayAbility> Ability, EAirAbilityInputID InputID)
{
	if (!AbilitySystemComponent || !HasAuthority() || !Ability) return;

	FGameplayAbilitySpec Spec(Ability);
	Spec.InputID = static_cast<int32>(InputID); // This is the missing link!

	AbilitySystemComponent->GiveAbility(Spec);
}


void AAirShooterCharacter::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}
	for (FGameplayAbilitySpecHandle Handle : AbilityHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(Handle);
	}
	SendAbilitiesChangedEvent();
}

void AAirShooterCharacter::SendAbilitiesChangedEvent()
{
		FGameplayEventData EventData;
		EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.AbilitiesChanged"));
		EventData.Instigator = this;
		EventData.Target = this;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void AAirShooterCharacter::ServerSendGameplayEventToSelf_Implementation(FGameplayEventData EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void AAirShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAirShooterCharacter, TeamByte);
}
