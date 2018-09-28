// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "queue"
#include "PlaneActor.generated.h"

UCLASS()
class AIRTRAFFIC_API APlaneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlaneActor();

	void SetStartPoint(FTransform Arg);
	void SetStartLoc(FVector Arg);
	void SetEndPoint(FTransform Arg);
	void SetEndLoc(FVector Arg);
	void SetSpeed(float Arg);
	void SetMachOne(float Arg);
	void SetName(FString Arg);
	void SetFlightLevel(float Arg);
	void SetEndFlightLevel(float Arg);
	void SetAltitudeChange(float Arg);
	void SetSpeedScale(float Arg);
	void AddTrajectory(FVector Arg);
	void RemoveTrajectory();
	void EmpthyTrajectory();
	void ShowSpeed();

	FTransform GetStartPoint() const;
	FTransform GetEndPoint() const;
	float GetSpeed() const;
	FString GetName() const;
	float GetFlightLevel() const;
	float GetEndFlightLevel() const;
	float GetMachOne() const;
	float GetAltitudeChange() const;
	float GetSpeedScale() const;
	FVector GetTrajectory() const;

private:

	void CalculatePath();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	FVector GetNewRotatedDestPoint(const FVector &Center, const FRotator &Rotation, const float &Distance);

	void ChangePointRot(float Value);

	UFUNCTION()
	void PlaneCommandProcessing(TArray<FString> Args);

	UFUNCTION(BlueprintCallable)
	void GiveInfo(FString &Name, float &FlightLevel, float &Speed);

	UFUNCTION(BlueprintCallable)
	void DrawTrajectory();

private:

	FTransform StartPos;
	FTransform EndPos;
	float speed;
	float RelSpeed;
	float AltitudeChange;
	FVector Path;
	FString Name;
	float FlightLevel;
	float EndFlightLevel;
	float MACH_ONE;
	float SpeedScale;
	std::queue<FVector> Trajectory;

public:
	UPROPERTY(BlueprintReadWrite)
	FRotator PlaneRotation;

};
