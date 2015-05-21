

#include "Scroller2D.h"
#include "AICharacter2D.h"
#include "AICharacter2DController.h"


AAICharacter2D::AAICharacter2D(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, bShowPathFinding(false)
	, bUseSteering(false)
	, PathColor(FColor::Red)
	, bUseCollisionAvoidance(false)
	, bUseArrival(false)
	, bUsePursuit(false)
{
	AIControllerClass = AAICharacter2DController::StaticClass();
	MaxThrust = 10.0f;
	MaxLookAhead = 32.f;
	MinCollisionRadius = 32.f;
	MaxAvoidanceForce = 10.f;

	ArrivalDistance = 160.f;
	
	//Movement = PCIP.CreateDefaultSubobject<UMovementComponent>(this, TEXT("Movement"));

	//AddComponent(FName("Movement"), false, FTransform(), NULL);
}


void AAICharacter2D::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("We are using AICharacter2D!"));
	}

}

void AAICharacter2D::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//CapsuleComponent->SetAllPhysicsRotation(FRotator(0, 0, 0));
}

void AAICharacter2D::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}

void AAICharacter2D::UpdateAnimation(bool bRunningUp)
{

}