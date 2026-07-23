// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/VoiceInterface.h"
#include "Engine/GameInstance.h"
#include "UObject/UObjectIterator.h"
#include "VOIPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class AIRSHOOTER_API UVOIPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "VOIP")
	static void ClearVoicePackets(UObject* WorldContextObject);
};
