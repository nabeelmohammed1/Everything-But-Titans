// Fill out your copyright notice in the Description page of Project Settings.


#include "AS_AbilitySystemComp.h"
#include "AirShooterCharacter.h"

UAS_AbilitySystemComp::UAS_AbilitySystemComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// Add this line to fix client-side tag/effect sync for players
	SetReplicationMode(EGameplayEffectReplicationMode::Full);
}

void UAS_AbilitySystemComp::BeginPlay()
{
	Super::BeginPlay();
}

void UAS_AbilitySystemComp::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	AAirShooterCharacter* Character = Cast<AAirShooterCharacter>(GetOwner());
	if (!Character) return;

	bool bAbilitiesChanged = false;
	if (LastActivationAbilities.Num() != ActivatableAbilities.Items.Num())
	{
		bAbilitiesChanged = true;
	}
	else
	{
		for (int32 i = 0; i < LastActivationAbilities.Num(); i++)
		{
			if (LastActivationAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
			{
				bAbilitiesChanged = false;
				break;
			}
		}
	}

	if (bAbilitiesChanged)
	{
		Character->SendAbilitiesChangedEvent();
		LastActivationAbilities = ActivatableAbilities.Items;
	}
}

void UAS_AbilitySystemComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

