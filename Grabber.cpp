// Copyright Kamikadze Studios 2022


#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

UGrabber::UGrabber()   											// Sets default values for this component's properties
{
	PrimaryComponentTick.bCanEverTick = true;    				// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features off to improve performance if you don't need them.
}

void UGrabber::BeginPlay()
{
	Super::BeginPlay();											// UE_LOG(LogTemp, Warning, TEXT("Grabber active"));
		
	FindPhysicsHandle();

	SetupInputComponent();
}

void UGrabber::FindPhysicsHandle()
{																										//check for the physics handle component																											
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr)  //you can alternatively use !PhysicsHandle
	{
		//physics is found.				
	}
	else
	{
		// // UE_LOG(LogTemp,Error, TEXT("PhysicsHandle component not working pn %s"),
		// *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}

}

void UGrabber::Grab()
	{
		
	

		FHitResult HitResult = GetFirstPhysicsBodyInReach();
		UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
		AActor* ActorHit = HitResult.GetActor();

	if(ActorHit)				//used to be HitResult.GetActor()  but now we initialized it up at AActor*																	// if we hit something then attach the physics handle
	{	
		if(!PhysicsHandle){return;} // when it gets to this line of code it will just jump out and it wont run the code underneath
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach()
		);
	}	
		
	}

void UGrabber::Release()
	{
		if(!PhysicsHandle){return;} // as said up there if we dont have physics handle without this code it would crash so now it wont
		PhysicsHandle->ReleaseComponent();
	}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PhysicsHandle){return;}
	if(PhysicsHandle->GrabbedComponent)                                                                           //If the physics handle is attached
	{
		PhysicsHandle->SetTargetLocation(GetPlayersReach());                                                          //move object we are holding
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	
	// DrawDebugLine(
	// 	GetWorld(),
	// 	PlayerViewPointLocation,
	// 	LineTraceEnd,
	// 	FColor(0,255,0),
	// 	false,
	// 	0.f,
	// 	0.f, 
	// 	4.f
	// );
	FHitResult Hit;                                        
	
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());                                                      // ray cast out to a certain distance (Reach)

	GetWorld()->LineTraceSingleByObjectType(			//theres always gonna be a world so no need to protect the pointer much like get owner
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;

	// AActor* ActorHit = Hit.GetActor();

	// if (ActorHit)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("we hit: %s"), *(ActorHit->GetName()));
	// }

	// return Hit;
}

FVector UGrabber::GetPlayersWorldPos() const
{
	FVector PlayerViewPointLocation;                                                                        // get players viewpoint
		FRotator PlayerViewPointRotation;

		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
			OUT PlayerViewPointLocation,
			OUT PlayerViewPointRotation);

	
	return PlayerViewPointLocation;
	// FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayersReach() const
	{
			FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;

		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
			OUT PlayerViewPointLocation,
			OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	return LineTraceEnd;

	}
