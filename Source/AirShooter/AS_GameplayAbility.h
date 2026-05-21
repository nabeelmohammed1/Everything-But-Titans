// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AirAttributes.h"
#include "AS_GameplayAbility.generated.h"

class AAirShooterCharacter;

UCLASS()
class AIRSHOOTER_API UAS_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInAbilitiesBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInHud = false;


protected:
	UFUNCTION(BlueprintPure, Category = "Ability|Helpers")
	UCharacterMovementComponent* GetCharacterMovement() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Helpers")
	float GetAttributeValue(FGameplayAttribute Attribute) const;

	UFUNCTION(BlueprintPure, Category = "Ability|Helpers")
	AAirShooterCharacter* GetOwningCharacter() const;

};

