// Fill out your copyright notice in the Description page of Project Settings.

#include "AirTrafficGameMode.h"
#include "PlaneActor.h"
#include "queue"

void AAirTrafficGameMode::InputProcessing(FString Arg)
{

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Arg);

	if (Arg == "Enter")
	{
		TArray<FString> Args;
		FString Del = " ";
		CommandLineText.ParseIntoArray(Args, TEXT(" "), true);
		for (auto x : Args)
		{
			x = x.TrimStartAndEnd();
		}
		ChangeCommandLineBool();
		CommandProcessing(Args);
	}
	else if (Arg == "Backspace")
	{
		if (CommandLineText.Len() != 0)
		{
			CommandLineText.RemoveAt(CommandLineText.Len() - 1);
		}
	}
	else if (Arg == "Hyphen")
	{
		CommandLineText.Append("-");
	}
	else if (Arg == "Equals")
	{
		CommandLineText.Append("=");
	}
	else if (Arg == "Space Bar")
	{
		CommandLineText.Append(" ");
	}
	else if (Arg == "Period")
	{
		CommandLineText.Append(".");
	}
	else if (Arg.Len() > 1)
	{

	}
	else
	{
		CommandLineText.Append(Arg);
	}

}

bool AAirTrafficGameMode::CheckAirPoints(const FString & Arg)
{

	for (auto x : PlanesRef)
	{
		if (Arg == x->GetName())
		{
			return true;
		}
	}
	return false;
}

APlaneActor * AAirTrafficGameMode::FindPlane(const FString & Arg)
{

	for (auto x : PlanesRef)
	{
		if (x->GetName() == Arg)
		{
			return x;
		}
	}

	return nullptr;
}

void AAirTrafficGameMode::CommandProcessing(TArray<FString> Args)
{
	if (Args.Num() < 1)
	{
		return;
	}
	if (Args[0] == "CREATE")
	{
		FuncCreate(Args);
		return;
	}
	else if (Args[0] == "Destroy")
	{
		FuncDestroy(Args);
	}
	else if (Args[0] == "DIRECT")
	{
		FuncReroute(Args);
	}
	else if (CheckAirPoints(Args[0]))
	{

		for (auto x : PlanesRef)
		{
			if (Args[0] == x->GetName())
			{
				x->PlaneCommandProcessing(Args);
			}
		}
	}
}

void AAirTrafficGameMode::FuncCreate(TArray<FString> Args)
{
	if (Args.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalied function sign."));
		return;
	}

	FTransform FirstPoint = GetAirPointLoc(Args[1]);
	if (FirstPoint.GetLocation() == FVector(0.0f, 0.0f, 0.0f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Air point %s not found."), *Args[1]);
		return;
	}

	FString PlaneName = "PlaceHolder";
	std::queue<FVector> tempLoc;
	for (int i = 2; i < Args.Num(); i++)
	{
		FTransform temp = GetAirPointLoc(Args[i]);
		if (temp.GetLocation() == FVector(0.0f, 0.0f, 0.0f))
		{
			PlaneName = Args[i];
			break;
		}
		tempLoc.push(temp.GetLocation());
	}

	if (PlaneName == "PlaceHolder")
	{
		PlaneName = GetRandomPlaneName();
		UE_LOG(LogTemp, Warning, TEXT("PlaneName: %s"), *PlaneName);
	}

	for (auto x : PlanesRef)
	{
		if (PlaneName == x->GetName())
		{
			UE_LOG(LogTemp, Warning, TEXT("Plane Already in flight."));
			return;
		}
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Name = FName(*PlaneName);

	if (!PlaneSpawnActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blueprint Not Set/Found."));
		return;
	}

	APlaneActor* ref = (APlaneActor*)GetWorld()->SpawnActor<APlaneActor>(PlaneSpawnActor, FirstPoint.GetLocation(), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);

	MACH_ONE = DistInGameAmtovUteka / 1000.0f;
	ref->SetSpeed(MACH_ONE * 0.7f);
	ref->SetSpeedScale(0.7f);
	ref->SetMachOne(MACH_ONE);
	ref->SetStartPoint(FirstPoint);

	int j = tempLoc.size();
	UE_LOG(LogTemp, Warning, TEXT("Points Trajectory: %i."), j);
	for (int i = 0; i < j; i++)
	{
		ref->AddTrajectory(tempLoc.front());
		tempLoc.pop();
	}

	ref->SetAltitudeChange(0.166666667f);
	ref->SetName(PlaneName);
	ref->SetFlightLevel(100.0f);
	ref->SetEndFlightLevel(100.0f);
	ref->SetActorTickEnabled(this->IsGamePaused);
	ref->RemoveTrajectory();
	AddInfoToPlane(ref);
	PlanesRef.Add(ref);

}

void AAirTrafficGameMode::FuncDestroy(TArray<FString> Args)
{
	if (Args.Num() > 2 || Args.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalied function sign."));
		return;
	}

	int temp = 0;
	bool check = false;
	for (auto x : PlanesRef)
	{
		if (Args[1] == x->GetName())
		{
			x->Destroy();
			temp = PlanesRef.Find(x);
			check = true;
		}
	}

	if (check)
	{
		PlanesRef.RemoveAtSwap(temp, 1, true);
	}

}

void AAirTrafficGameMode::FuncReroute(TArray<FString> Args)
{
	if (Args.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalied function sign."));
		return;
	}

	FString PlaneName = "PlaceHolder";
	std::queue<FVector> tempLoc;
	for (int i = 1; i < Args.Num(); i++)
	{
		FTransform temp = GetAirPointLoc(Args[i]);
		if (temp.GetLocation() == FVector(0.0f, 0.0f, 0.0f))
		{
			PlaneName = Args[i];
			break;
		}
		tempLoc.push(temp.GetLocation());
	}

	APlaneActor* tempPlane = FindPlane(PlaneName);

	if (tempPlane)
	{
		tempPlane->EmpthyTrajectory();
		int j = tempLoc.size();
		UE_LOG(LogTemp, Warning, TEXT("Points Trajectory: %i."), j);
		for (int i = 0; i < j; i++)
		{
			tempPlane->AddTrajectory(tempLoc.front());
			tempLoc.pop();
		}
		tempPlane->RemoveTrajectory();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No plane found."));
		return;
	}

}



