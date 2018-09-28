// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AirTrafficGameMode.generated.h"

/**
 * 
 */

class APlaneActor;

UCLASS()
class AIRTRAFFIC_API AAirTrafficGameMode : public AGameMode
{
	GENERATED_BODY()

private:


public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	FTransform GetAirPointLoc(const FString &Arg);

	UFUNCTION(BlueprintImplementableEvent)
	FString GetRandomPlaneName();

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeCommandLineBool();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PauseAirplanes();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddInfoToPlane(const APlaneActor *Arg);

	bool CheckAirPoints(const FString &Arg);

	APlaneActor* FindPlane(const FString &Arg);

private:

	UFUNCTION()
	void CommandProcessing(TArray<FString> Args);

	UFUNCTION(BlueprintCallable)
	void InputProcessing(FString Arg);

	UFUNCTION()
	void FuncCreate(TArray<FString> Args);

	UFUNCTION()
	void FuncDestroy(TArray<FString> Args);

	UFUNCTION()
	void FuncReroute(TArray<FString> Args);

public:

	UPROPERTY(BlueprintReadWrite)
	FString CommandLineText;

	UPROPERTY(BlueprintReadWrite)
	TArray< APlaneActor* > PlanesRef;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<APlaneActor> PlaneSpawnActor;

	UPROPERTY(BlueprintReadWrite)
	float DistInGameAmtovUteka;

	UPROPERTY(BlueprintReadWrite)
	float MACH_ONE;

	UPROPERTY(BlueprintReadWrite)
	bool IsGamePaused;
};
