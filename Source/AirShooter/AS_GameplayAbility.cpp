// Fill out your copyright notice in the Description page of Project Settings.


#include "AS_GameplayAbility.h"
#include "GameFramework/Character.h"
#include "AS_AbilitySystemComp.h"
#include "AirShooterCharacter.h"

UCharacterMovementComponent* UAS_GameplayAbility::GetCharacterMovement() const
{
    // The ActorInfo struct holds the Avatar (the physical Pawn/Character)
    if (const FGameplayAbilityActorInfo* Info = CurrentActorInfo)
    {
        // GAS already stores the Avatar as a TWeakObjectPtr<AActor>
        // We cast the Avatar to a Character to get the movement component
        if (ACharacter* Character = Cast<ACharacter>(Info->AvatarActor.Get()))
        {
            return Character->GetCharacterMovement();
        }
    }
    return nullptr;
}

float UAS_GameplayAbility::GetAttributeValue(FGameplayAttribute Attribute) const
{
    if (UAS_AbilitySystemComp* ASC = Cast<UAS_AbilitySystemComp>(GetAbilitySystemComponentFromActorInfo()))
    {
        return ASC->GetNumericAttribute(Attribute);
    }
	return 0.f;
}

AAirShooterCharacter* UAS_GameplayAbility::GetOwningCharacter() const
{
    if (const FGameplayAbilityActorInfo* Info = CurrentActorInfo)
    {
        return Cast<AAirShooterCharacter>(Info->AvatarActor.Get());
    }
    return nullptr;
}
