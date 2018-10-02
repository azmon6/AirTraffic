// Fill out your copyright notice in the Description page of Project Settings.

#include "PlaneActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/LineBatchComponent.h"

// Sets default values
APlaneActor::APlaneActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APlaneActor::SetStartPoint(FTransform Arg)
{
	this->StartPos = Arg;
	return;
}

void APlaneActor::SetStartLoc(FVector Arg)
{
	this->StartPos.SetLocation(Arg);
	return;
}

void APlaneActor::SetEndPoint(FTransform Arg)
{
	this->EndPos = Arg;
	CalculatePath();
	return;
}

void APlaneActor::SetEndLoc(FVector Arg)
{
	this->EndPos.SetLocation(Arg);
	CalculatePath();
	return;
}

void APlaneActor::SetSpeed(float Arg)
{
	this->speed = Arg;
	CalculatePath();
	return;
}

void APlaneActor::SetMachOne(float Arg)
{
	this->MACH_ONE = Arg;
	return;
}

void APlaneActor::SetName(FString Arg)
{
	this->Name = Arg;
	return;
}

void APlaneActor::SetFlightLevel(float Arg)
{
	this->FlightLevel = Arg;
	return;
}

void APlaneActor::SetEndFlightLevel(float Arg)
{
	this->EndFlightLevel = Arg;
	if (EndFlightLevel - FlightLevel < 0)
	{
		this->SetAltitudeChange(this->GetAltitudeChange() * -1.0f);
	}
	return;
}

void APlaneActor::SetAltitudeChange(float Arg)
{
	this->AltitudeChange = Arg;
	return;
}

void APlaneActor::SetSpeedScale(float Arg)
{
	this->SpeedScale = Arg;
	return;
}

void APlaneActor::AddTrajectory(FVector Arg)
{
	this->Trajectory.push(Arg);
	return;
}

void APlaneActor::RemoveTrajectory()
{
	if (!this->Trajectory.empty())
	{
		this->SetEndLoc(this->GetTrajectory());
		this->Trajectory.pop();
	}
	return;
}

void APlaneActor::EmpthyTrajectory()
{
	while (!this->Trajectory.empty())
	{
		this->Trajectory.pop();
	}
	return;
}

void APlaneActor::ShowSpeed()
{
	FVector temp = UKismetMathLibrary::GetForwardVector(PlaneRotation);
	FVector temp2 = this->GetActorLocation() + temp * GetSpeed()*60.0f;
	DrawDebugLine(GetWorld(), this->GetActorLocation(), temp2, FColor(255, 0, 0), false, -1, 0, 3.0f);
}

FTransform APlaneActor::GetStartPoint() const
{
	return this->StartPos;
}

FTransform APlaneActor::GetEndPoint() const
{
	return this->EndPos;
}

float APlaneActor::GetSpeed() const
{
	return this->speed;
}

FString APlaneActor::GetName() const
{
	return this->Name;
}

float APlaneActor::GetFlightLevel() const
{
	return this->FlightLevel;
}

float APlaneActor::GetEndFlightLevel() const
{
	return this->EndFlightLevel;
}

float APlaneActor::GetMachOne() const
{
	return this->MACH_ONE;
}

float APlaneActor::GetAltitudeChange() const
{
	return this->AltitudeChange;
}

float APlaneActor::GetSpeedScale() const
{
	return this->SpeedScale;
}

FVector APlaneActor::GetTrajectory() const
{
	return this->Trajectory.front();
}

void APlaneActor::CalculatePath()
{
	PlaneRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetEndPoint().GetLocation());
	FVector From = this->GetActorLocation();
	FVector To = this->GetEndPoint().GetLocation();

	this->Path = FVector(From.X - To.X, From.Y - To.Y, From.Z - To.Z);

	this->RelSpeed = GetSpeed() / FVector::Dist(From, To) * -1.0f;

	return;
}

// Called when the game starts or when spawned
void APlaneActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("I LIVE!"));
}

// Called every frame
void APlaneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->GetEndFlightLevel() - this->GetFlightLevel() != 0.0f)
	{
		float temp = this->GetFlightLevel() + this->GetAltitudeChange()*DeltaTime;
		this->SetFlightLevel(temp);
		if (FMath::Abs(this->GetEndFlightLevel() - this->GetFlightLevel()) < 0.01f)
		{
			this->SetFlightLevel(this->GetEndFlightLevel());
			this->SetAltitudeChange(0.166666667f);
		}
	}

	float temp1 = PlaneRotation.Yaw;
	float temp3 = EndRotation;
	if (temp1 < 0.0f) {temp1 = 360.0f + temp1;}
	if (temp3 < 0.0f) { temp3 = 360.0f + temp3; }
	if (temp1 != temp3)
	{
		this->ChangePointRot(3.0f*DeltaTime*RotationClockwise);
		if (FMath::Abs(temp1 - temp3) <= 0.3f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Change yaw. %f"), temp1);
			PlaneRotation.Yaw = EndRotation;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Change endrot. %f"), temp3);
		//UE_LOG(LogTemp, Warning, TEXT("Change yaw. %f"), temp1);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Flight Level: %f!"),this->GetFlightLevel());

	FVector New = GetActorLocation() + this->Path * this->RelSpeed * DeltaTime;

	this->SetActorLocation(New);

	FVector2D temp(GetActorLocation().X, GetActorLocation().Y);
	FVector2D temp2(GetEndPoint().GetLocation().X, GetEndPoint().GetLocation().Y);

	if (FVector2D::Distance(temp, temp2) < 0.3f)
	{
		this->RemoveTrajectory();
		UE_LOG(LogTemp, Warning, TEXT("Got to End Point!"));
	}

	ShowSpeed();

}

void APlaneActor::ChangePointRot(float Value)
{
	/*float HereX = GetActorLocation().X;
	float HereY = GetActorLocation().Y;

	float ThereX = GetEndPoint().GetLocation().X;
	float ThereY = GetEndPoint().GetLocation().Y;

	float sin = FMath::Sin(FMath::DegreesToRadians(Value));
	float cos = FMath::Cos(FMath::DegreesToRadians(Value));

	float newX = cos * (ThereX - HereX) - sin * (ThereY - HereY) + HereX;
	float newY = sin * (ThereX - HereX) + cos * (ThereY - HereY) + HereY;

	FVector NewPoint(newX, newY, GetActorLocation().Z);*/

	float Distance = FVector::Dist(GetActorLocation(), GetEndPoint().GetLocation());
	PlaneRotation = FRotator(0.0f, PlaneRotation.Yaw+Value, 0.0f);

	FVector temp = UKismetMathLibrary::GetForwardVector(PlaneRotation);
	FVector temp2 = this->GetActorLocation() + temp * GetSpeed() * 60.0f;

	SetEndLoc(temp2);
}

void APlaneActor::PlaneCommandProcessing(TArray<FString> Args)
{
	if (Args.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("No command enterned."));
		return;
	}

	if (Args[1] == "TURN")
	{
		if (Args.Num() > 4 || Args.Num() < 4)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong function sign."));
			return;
		}

		if (Args[2] == "LEFT")
		{
			//ChangePointRot(FCString::Atof(*Args[3]));
			this->RotationClockwise = -1.0f;
			this->EndRotation = FCString::Atof(*Args[3]);
		}
		else if (Args[2] == "RIGHT")
		{
			//ChangePointRot(FCString::Atof(*Args[3]));
			this->RotationClockwise = 1.0f;
			this->EndRotation = FCString::Atof(*Args[3]);
		}

		this->EmpthyTrajectory();

	}
	else if (Args[1] == "INCREASE")
	{
		if (Args.Num() > 4 || Args.Num() < 4)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong function sign."));
			return;
		}

		this->SetSpeed(this->GetMachOne() * FCString::Atof(*Args[3]));
		SetSpeedScale(FCString::Atof(*Args[3]));

	}
	else if (Args[1] == "REDUCE")
	{
		if (Args.Num() > 4 || Args.Num() < 4)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong function sign."));
			return;
		}

		this->SetSpeed(this->GetMachOne() * FCString::Atof(*Args[3]));
		SetSpeedScale(FCString::Atof(*Args[3]));
	}
	else if (Args[1] == "CLIMB")
	{
		if (Args.Num() > 3 || Args.Num() < 3)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong function sign."));
			return;
		}
		this->SetAltitudeChange(0.166666667f);
		if (FCString::Atof(*Args[2]) <= this->GetFlightLevel())
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't climb."));
			return;
		}
		this->SetEndFlightLevel(FCString::Atof(*Args[2]));
	}
	else if (Args[1] == "DESCENT")
	{
		if (Args.Num() > 4 || Args.Num() < 3)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong function sign."));
			return;
		}
		this->SetAltitudeChange(0.166666667f);
		if (Args.Num() == 4 && Args[3] == "2")
		{
			this->SetAltitudeChange(this->GetAltitudeChange()*2.0f);
		}
		if (FCString::Atof(*Args[2]) >= this->GetFlightLevel())
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't descent."));
			return;
		}
		this->SetEndFlightLevel(FCString::Atof(*Args[2]));
	}
	else if (Args[1] == "SET")
	{
		if (Args.Num() < 4)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong function sign."));
			return;
		}
		if (Args[2] == "FL")
		{
			this->SetFlightLevel(FCString::Atof(*Args[3]));
			this->SetEndFlightLevel(FCString::Atof(*Args[3]));
		}
	}
}

void APlaneActor::GiveInfo(FString & Name, float & FlightLevel, float &Speed)
{
	Name = GetName();
	FlightLevel = GetFlightLevel();
	Speed = GetSpeedScale();
}

void APlaneActor::DrawTrajectory()
{
	DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetEndPoint().GetLocation(), FColor(255, 0, 0), false, -1, 0, 3.0f);
	std::queue<FVector>temp = Trajectory;
	if (temp.empty())
	{
		return;
	}
	FVector prev = this->GetEndPoint().GetLocation();

	int j = temp.size();
	for (int i = 0; i < j; i++)
	{
		DrawDebugLine(GetWorld(), prev, temp.front(), FColor(255, 0, 0), false, -1, 0, 3.0f);
		prev = temp.front();
		temp.pop();
	}

}

void APlaneActor::DrawDebugLines(const FVector & Arg1, const FVector & Arg2)
{
	DrawDebugLine(GetWorld(), Arg1, Arg2, FColor(255, 0, 0), false, -1, 0, 3.0f);
}
