

#pragma once

#include "PaperCharacter.h"
#include "Tileset2D.h"
#include "AICharacter2D.generated.h"

/**
 * 
 */
UCLASS()
class SCROLLER2D_API AAICharacter2D : public APaperCharacter
{
	GENERATED_UCLASS_BODY()

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	//TSubobjectPtr<UMovementComponent> Movement;

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UPaperFlipbook* IdleAnimation;

	void UpdateAnimation(bool bRunningUp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	bool bShowPathFinding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	FColor PathColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	bool bUseSteering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	bool bUseCollisionAvoidance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	bool bUsePursuit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	bool bUseArrival;

	/**Distance to start slowing down*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	float ArrivalDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	float MaxThrust;

	/**Distance for bot to look ahead for collisions*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	float MaxLookAhead;

	/**Minimum collision distance between ahead vector and objects*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	float MinCollisionRadius;

	/**Maximum force to apply to collision avoidance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	float MaxAvoidanceForce;

	virtual void Tick(float DeltaSeconds) override;

	FVector ThrustForce;
	
};
