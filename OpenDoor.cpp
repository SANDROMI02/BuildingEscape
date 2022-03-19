// Copyright Kamikadze Studios 2022


#include "OpenDoor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw; // OpenAngle = InitialYaw + OpenAngle;

	FindPressurePlate();

	FindAudioComponent();

	// ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void UOpenDoor::FindPressurePlate()
{
	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error , TEXT("%s pressureplate has not been set"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindAudioComponent()

{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if(!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Audio component not found on %s"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TotalMassOfActors() > MassToOpenDoor)  //PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if(GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)   //if the door has been open longer than DoorCloseDelay
		{
		CloseDoor(DeltaTime);
		}
	}
	
	// alternative CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle,DeltaTime * 1.f);

	// float CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	// FRotator OpenDoor(0.f,OpenAngle,0.f);
	// OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, OpenAngle,DeltaTime, 2);
	// GetOwner()->SetActorRotation(OpenDoor);

	 // (current yaw)starting yaw, target yaw, 0-1
	
	// // FRotator OpenDoor(0.f,90.f,0.f);
	// // GetOwner()->SetActorRotation(OpenDoor);
	
	// FRotator OpenDoor = GetOwner()->GetActorRotation();
	// OpenDoor.Yaw = 90.f;
	
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetActorRotation().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("yaw is %f"), GetOwner()->GetActorRotation().Yaw);

	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle,DeltaTime, DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);	

	CloseDoorSound = false;
	if(!AudioComponent){return;}
	if(!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}

	

}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, InitialYaw,DeltaTime, DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if(!AudioComponent) {return;}
	if(!CloseDoorSound)
	{
	AudioComponent->Play();
	CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 60.f;

	//find all overlapping actors
	
	TArray<AActor*>OverlappingActors;
	if(!PressurePlate){return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	//add up their masses

	for(AActor*Actor:OverlappingActors)
		{
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
			// UE_LOG(LogTemp, Warning, TEXT("%s is on the pressusureplate"), *Actor->GetName());
		}

	return TotalMass;
}
