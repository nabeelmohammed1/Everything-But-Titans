// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AirAttributes.generated.h"

/**
 * 
 */
UCLASS()
class AIRSHOOTER_API UAirAttributes : public UAttributeSet
{
	GENERATED_BODY()

public:	
	UAirAttributes();
	//Health Attribute
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UAirAttributes, Health);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UAirAttributes, MaxHealth);

	//Stamina Attribute
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS_BASIC(UAirAttributes, Stamina);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS_BASIC(UAirAttributes, MaxStamina);

	//Shield Attribute
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS_BASIC(UAirAttributes, Shield);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS_BASIC(UAirAttributes, MaxShield);

public:

	//Functions to call when the attributes are updated on clients. This is where we would update any UI or other visuals based on the new attribute values.
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(UAirAttributes, Health, OldHealth);
	}

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(UAirAttributes, MaxHealth, OldMaxHealth);
	}

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(UAirAttributes, Stamina, OldStamina);
	}

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(UAirAttributes, MaxStamina, OldMaxStamina);
	}

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(UAirAttributes, Shield, OldShield);
	}

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(UAirAttributes, MaxShield, OldMaxShield);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
};
