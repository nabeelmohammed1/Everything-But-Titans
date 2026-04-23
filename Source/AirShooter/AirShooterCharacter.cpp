// Fill out your copyright notice in the Description page of Project Settings.


#include "AirShooterCharacter.h"
#include "AirAttributes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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


}

// Called when the game starts or when spawned
void AAirShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent) 
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
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
}

void AAirShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
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
