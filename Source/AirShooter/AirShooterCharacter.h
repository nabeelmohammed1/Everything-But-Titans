// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/WidgetComponent.h"
#include "AirShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EAirAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Grapple,
	Blink,
	Clone,
	Jump,
	Sprint,
	Slide,
	WallRun,
	Flash,
	Nade,
};

USTRUCT(BlueprintType)
struct FStartingAbilityInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAirAbilityInputID InputID = EAirAbilityInputID::None;
};

class UAbilitySystemComponent;

UCLASS()
class AIRSHOOTER_API AAirShooterCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAirShooterCharacter();

	//ability system component (exposed to BP, but defined in cpp)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAirAttributes* AirAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team", Replicated ,meta = (ExposeOnSpawn = "true"))
	uint8 TeamByte = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	FText PlayerName = FText::FromString("Player");

	//UI Related properties

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* NameplateWidgetComponent;

	/**
	 * How far away (in cm) another player can be before their nameplate is
	 * automatically hidden, even if they are technically visible.
	 * Default: 5000 cm (50 m). Tweak per-character in BP.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float NameplateMaxDistance = 5000.0f;

	//How often to check for visiblity

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float NameplateCheckInterval = 0.5f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	EGameplayEffectReplicationMode ASCReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<FStartingAbilityInfo> StartingAbilities;

	//Nameplate related Functions

	/** Cached list of other players — refreshed every few seconds. */
	UPROPERTY()
	TArray<AAirShooterCharacter*> CachedOtherPlayers;

	FTimerHandle NameplateCheckTimerHandle;
	FTimerHandle PlayerCacheRefreshTimerHandle;


	//Tracking if timer has started since we are calling it in 2 places.
	bool bNameplateTimerStarted = false;

	/** Rebuilds CachedOtherPlayers. Called on a slow (5 s) timer. */
	void RefreshPlayerCache();

	/** Main per-interval visibility update. Only runs on the local client. */
	void UpdateNameplateVisibility();

	/**
	 * Starts the nameplate check and cache-refresh timers.
	 * Called from PossessedBy (covers the listen-server host) and from
	 * OnRep_PlayerState (covers all remote clients). This ensures the
	 * controller is fully valid before the first timer fires.
	 * Safe to call multiple times – guarded by bNameplateTimersStarted.
	 */

	void StartNameplateTimers();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;	
	virtual void OnRep_PlayerState() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IAbilitySystemInterface implementation

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "GAS")
	void GrantStartingAbilities();

	UFUNCTION(BlueprintCallable, Category = "GAS")
	void GrantAbilityWithInput(TSubclassOf<UGameplayAbility> Ability, EAirAbilityInputID InputID);

	UFUNCTION(BlueprintCallable, Category = "GAS")
	void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "GAS")
	void SendAbilitiesChangedEvent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GAS")
	void ServerSendGameplayEventToSelf(FGameplayEventData EventData);

};
