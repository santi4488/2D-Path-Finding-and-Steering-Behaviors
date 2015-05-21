

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Tileset2D.h"
#include "BTTask_MoveTo2D.generated.h"

struct FBTMoveTo2DMemory
{
	FAIRequestID MoveRequestID;

	uint8 bWaitingForPath : 1;
};

/**
 * Blackboard entry must be set to a vector location
 * Uses 2D A* pathfinding to reach the goal location
 * on the tilemap
 */
UCLASS()
class SCROLLER2D_API UBTTask_MoveTo2D : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Node, meta = (ClampMin = "0.0"))
	float AcceptableRadius;

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent * OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory)override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void DescribeRuntimeValues(const class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;

	virtual void TickTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	EBTNodeResult::Type PerformMoveTask2D(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory);

	void MoveTo(FVector pos);

	TArray<FVector> path;

	UPROPERTY(Category = Node, EditAnywhere)
	bool bAllowStrafe;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif
	
};
