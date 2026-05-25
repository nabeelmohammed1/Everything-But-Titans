// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorHudWidget.h"
#include "AirShooterCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerState.h"

void USpectatorHudWidget::SetSpectatedPawn(APawn* NewPawn)
{
	UnbindFromCurrentPawn();
	SpectatedPawn = NewPawn;
	LastUsedAbilityHandle = FGameplayAbilitySpecHandle();

	if (NewPawn)
	{
		if(APlayerState* PS = NewPawn->GetPlayerState())
			SpectatedPlayerName = PS->GetPlayerName();

		BindToNewPawn(NewPawn);
	}
	OnSpectatedPawnChanged(NewPawn);
}

void USpectatorHudWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!SpectatedPawn.IsValid()) return;

	RefreshAbilities();

	if(APlayerState* PS = SpectatedPawn->GetPlayerState())
	{
		OnUpdateKills(PS);
	}

	OnUpdateAmmo(SpectatedPawn.Get());

	OnUpdateAbilities(AbilityDisplayInfos);
}

void USpectatorHudWidget::RefreshAbilities()
{
    AAirShooterCharacter* Character = Cast<AAirShooterCharacter>(SpectatedPawn.Get());
    if (!Character) return;

    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (!ASC) return;

    AbilityDisplayInfos.Empty();

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (!Spec.Ability) continue;

        FAbilityDisplayInfo Info;
        Info.AbilityName = Spec.Ability->GetClass()->GetName();
        Info.bWasLastUsed = (Spec.Handle == LastUsedAbilityHandle);

        if (const UGameplayEffect* CDGE = Spec.Ability->GetCooldownGameplayEffect())
        {
            FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
                CDGE->InheritableOwnedTagsContainer.CombinedTags);

            TArray<float> Remaining = ASC->GetActiveEffectsTimeRemaining(Query);
            TArray<float> Durations = ASC->GetActiveEffectsDuration(Query);

            if (Remaining.Num() > 0)
            {
                Info.CooldownRemaining = Remaining[0];
                Info.CooldownTotal = FMath::Max(Durations[0], 1.f);
                Info.bIsOnCooldown = Info.CooldownRemaining > 0.f;
            }
        }

        AbilityDisplayInfos.Add(Info);
    }
}

void USpectatorHudWidget::BindToNewPawn(APawn* Pawn)
{
    AAirShooterCharacter* Character = Cast<AAirShooterCharacter>(Pawn);
    if (!Character) return;

    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (!ASC) return;

    AbilityActivatedHandle = ASC->AbilityActivatedCallbacks.AddUObject(
        this, &USpectatorHudWidget::OnAbilityActivated);
}

void USpectatorHudWidget::UnbindFromCurrentPawn()
{
    if (!SpectatedPawn.IsValid()) return;

    AAirShooterCharacter* Character = Cast<AAirShooterCharacter>(SpectatedPawn.Get());
    if (!Character) return;

    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (ASC && AbilityActivatedHandle.IsValid())
        ASC->AbilityActivatedCallbacks.Remove(AbilityActivatedHandle);
}

void USpectatorHudWidget::OnAbilityActivated(UGameplayAbility* Ability)
{
    if (!Ability) return;

    AAirShooterCharacter* Character = Cast<AAirShooterCharacter>(SpectatedPawn.Get());
    if (!Character) return;

    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (!ASC) return;

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.Ability == Ability)
        {
            LastUsedAbilityHandle = Spec.Handle;
            break;
        }
    }
}

