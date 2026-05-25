// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayAbilitySpec.h"
#include "SpectatorHudWidget.generated.h"

USTRUCT(BlueprintType)
struct FAbilityDisplayInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FString AbilityName;
	UPROPERTY(BlueprintReadOnly) float CooldownRemaining = 0.f;
	UPROPERTY(BlueprintReadOnly) float CooldownTotal = 1.f;
	UPROPERTY(BlueprintReadOnly) bool bIsOnCooldown = false;
	UPROPERTY(BlueprintReadOnly) bool bWasLastUsed = false;
};

/**
 * 
 */
UCLASS()
class AIRSHOOTER_API USpectatorHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSpectatedPawn(APawn* Pawn);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // ── Data exposed to UMG bindings ─────────────────────────────────────────

    UPROPERTY(BlueprintReadOnly, Category = "Spectator")
    FString SpectatedPlayerName;

    UPROPERTY(BlueprintReadOnly, Category = "Spectator")
    TArray<FAbilityDisplayInfo> AbilityDisplayInfos;

    // ── Blueprint Implementable Events ───────────────────────────────────────

    /** Fires every tick. Cast SpectatedPlayerState to your BP PlayerState
     *  and read your Kills variable to update the kills text widget. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Spectator")
    void OnUpdateKills(APlayerState* SpectatedPlayerState);

    /** Fires every tick. Cast SpectatedCharacter to your BP Character,
     *  get your WeaponComponent, and read CurrentAmmo / MaxAmmo. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Spectator")
    void OnUpdateAmmo(APawn* SpectatedCharacter);

    /** Fires when the spectated player switches — use to reset UI state. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Spectator")
    void OnSpectatedPawnChanged(APawn* NewPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spectator")
    void OnUpdateAbilities(const TArray<FAbilityDisplayInfo>& Abilities);

private:
    void RefreshAbilities();
	void BindToNewPawn(APawn* NewPawn);
	void UnbindFromCurrentPawn();
    void OnAbilityActivated(UGameplayAbility* Ability);

	UPROPERTY()
	TWeakObjectPtr<APawn> SpectatedPawn;

    FGameplayAbilitySpecHandle LastUsedAbilityHandle;
    FDelegateHandle AbilityActivatedHandle;
	
};
