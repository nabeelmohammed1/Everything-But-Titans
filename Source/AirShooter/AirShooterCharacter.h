// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team", meta = (ExposeOnSpawn = "true"))
	uint8 TeamByte = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	EGameplayEffectReplicationMode ASCReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<FStartingAbilityInfo> StartingAbilities;


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
