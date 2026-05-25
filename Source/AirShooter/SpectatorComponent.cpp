// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorComponent.h"
#include "SpectatorHudWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "AirShooterCharacter.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
USpectatorComponent::USpectatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USpectatorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USpectatorComponent::EnterSpectatorMode()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	if(!SpectatorWidget && SpectatorWidgetClass)
	{
		SpectatorWidget = CreateWidget<USpectatorHudWidget>(PC, SpectatorWidgetClass);
	}

	bIsSpectating = true;

	TArray<APawn*> Players = GetAllAlivePlayers();
	if (Players.IsEmpty()) return;
	
	CurrentSpectatedIndex = 0;
	SpecatePlayer(Players[0]);

	if (SpectatorWidget && !SpectatorWidget->IsInViewport())
	{
		SpectatorWidget->AddToViewport(10);
	}
}

void USpectatorComponent::ExitSpectatorMode()
{
	bIsSpectating = false;
	CurrentSpectatedPawn.Reset();

	if (SpectatorWidget && SpectatorWidget->IsInViewport())
	{
		SpectatorWidget->RemoveFromViewport();
	}

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC && PC->GetPawn())
	{
		PC->SetViewTarget(PC->GetPawn());
	}
}

void USpectatorComponent::SpectateNextPlayer()
{
	if (!bIsSpectating) return;

	TArray<APawn*> Players = GetAllAlivePlayers();
	if (Players.IsEmpty()) return;

	CurrentSpectatedIndex = (CurrentSpectatedIndex + 1) % Players.Num();
	SpecatePlayer(Players[CurrentSpectatedIndex]);

}

void USpectatorComponent::SpectatePreviousPlayer()
{
	if (!bIsSpectating) return;
	TArray<APawn*> Players = GetAllAlivePlayers();
	if (Players.IsEmpty()) return;
	CurrentSpectatedIndex = (CurrentSpectatedIndex - 1 + Players.Num()) % Players.Num();
	SpecatePlayer(Players[CurrentSpectatedIndex]);
}

void USpectatorComponent::SpecatePlayer(APawn* Target)
{
	if (!Target) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;	

	CurrentSpectatedPawn = Target;

	FViewTargetTransitionParams Params;
	Params.BlendTime = 0.3f;
	Params.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseInOut;
	PC->SetViewTarget(Target, Params);

	if(SpectatorWidget)
	{
		SpectatorWidget->SetSpectatedPawn(Target);
	}
}

TArray<APawn*> USpectatorComponent::GetAllAlivePlayers() const
{
	TArray<APawn*> Result;
	APlayerController* OwningPC = Cast<APlayerController>(GetOwner());
	APawn* OwningPawn = OwningPC ? OwningPC->GetPawn() : nullptr;

	for (TActorIterator<AAirShooterCharacter> It(GetWorld()); It; ++It)
	{
		AAirShooterCharacter* Character = *It;
		if (!Character || !IsValid(Character)) continue;
		if (Character == OwningPawn) continue;  // skip self

		Result.Add(Character);
	}
	return Result;
}