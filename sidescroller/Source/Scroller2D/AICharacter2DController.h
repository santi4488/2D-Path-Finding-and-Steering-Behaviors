

#pragma once

#include "AIController.h"
#include "Character2D.h"
#include "Tileset2D.h"
#include "AICharacter2D.h"
#include "AICharacter2DController.generated.h"

/**
 * 
 */
UCLASS()
class SCROLLER2D_API AAICharacter2DController : public AAIController
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Transient)
	TSubobjectPtr<class UBlackboardComponent> BlackboardComp;

	UPROPERTY(Transient)
	TSubobjectPtr<class UBehaviorTreeComponent> BehaviorComp;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	bool bShowPathFinding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	FColor PathColor;

	UMovementComponent * MovementComp;

	UCapsuleComponent* CapsuleComp;

	/**Pointer to navigation component for the bot*/
	ATileset2D* Navigation;

	TArray<FVector> FindPath2D(FVector GoalLocation);

	virtual void Tick(float DeltaSeconds) override;

	virtual void Possess(class APawn *InPawn);
	virtual void BeginInactiveState() override;

	TArray<FVector> path;
	
	void Respawn();

	bool HasReached2D(FVector TestPoint, float AcceptanceRadius);

	void SetEnemy(class APawn *InPawn);

	class ACharacter2D* GetEnemy() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool SearchForEnemyWithLOS(ACharacter2D* ExcludeEnemy);

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void SearchForEnemy();

	/**Returns true if there is a valid path
	* to the enemy
	*/
	UFUNCTION(BlueprintCallable, Category = Navigation)
	bool SetPath(FVector end);

	/**Returns the key id for path node*/
	UFUNCTION(BlueprintCallable, Category = BlackBoardAsset)
	uint8 GetPathKeyID();

	UFUNCTION(BlueprintCallable, Category = Navigation)
	TArray<FVector> GetPath();

	/**This is responsible for updating the path as the bot moves along and
	reaches each node in the path*/
	UFUNCTION(BlueprintCallable, Category = Navigation)
	void FollowPath();

	UFUNCTION(BlueprintCallable, Category = Navigation)
	bool LineOfSight();

	UFUNCTION(BlueprintCallable, Category = Navigation)
	EPathFollowingRequestResult::Type FollowPath2D(const FVector &Dest, float AcceptanceRadius);

	void DrawPath();

	EPathFollowingRequestResult::Type MoveToLocation2D(const FVector& Dest, float AcceptanceRadius);

	FVector steering;

	

	bool bUseSteering;



protected:

	bool LOSTrace(ACharacter2D* InEnemyChar) const;

	uint8 EnemyKeyID;
	uint8 EnemyLocationID;


	uint8 PathNodeID;

private:
	bool bIsPathComplete;
	FVector Force;
	FVector AvoidanceForce;
	float maxAvoidanceForce;
	float MinCollisionRadius;
	float MaxLookAhead;
	bool bUseCollision;
	bool bUseArrival;

	float timeToNextNode;
	float ArrivalDistance;


	//calculates force to
	FVector seek();
	FVector queue();
	void Collision(AAICharacter2D* MyPawn);
	void Pursuit(AAICharacter2D* MyPawn);


};
