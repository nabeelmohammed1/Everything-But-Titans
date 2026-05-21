// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "AirShooterCharacter.h"
#include "WorldspaceWidget.generated.h"

UCLASS()
class AIRSHOOTER_API AWorldspaceWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldspaceWidget();

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
