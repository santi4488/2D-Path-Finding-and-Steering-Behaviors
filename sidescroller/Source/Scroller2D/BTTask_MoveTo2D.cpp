

#include "Scroller2D.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter2DController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BTTask_MoveTo2D.h"


UBTTask_MoveTo2D::UBTTask_MoveTo2D(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, AcceptableRadius(0.f)
	, bAllowStrafe(false)
{
	NodeName = "MoveTo2D";
	bNotifyTick = true;

	//accept only vectors
	BlackboardKey.AddVectorFilter(this);
}

void UBTTask_MoveTo2D::TickTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTMoveTo2DMemory* MyMemory = (FBTMoveTo2DMemory*)NodeMemory;
	
	AAICharacter2DController* MyController = OwnerComp ? Cast<AAICharacter2DController>(OwnerComp->GetOwner()) : NULL;
	if (MyController && !MyController->ShouldPostponePathUpdates())
	{
		UE_LOG(PathFollowing, Log, TEXT("Pathfinding requests are unlocked!"));
		MyMemory->bWaitingForPath = false;

		const EBTNodeResult::Type NodeResult = PerformMoveTask2D(OwnerComp, NodeMemory);
		if (NodeResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, NodeResult);
		}
	}
	
}

EBTNodeResult::Type UBTTask_MoveTo2D::ExecuteTask(class UBehaviorTreeComponent * OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	FBTMoveTo2DMemory* MyMemory = (FBTMoveTo2DMemory*)NodeMemory;
	AAICharacter2DController* MyController = OwnerComp ? Cast<AAICharacter2DController>(OwnerComp->GetOwner()) : NULL;

	MyMemory->bWaitingForPath = MyController->ShouldPostponePathUpdates();

	if (!MyMemory->bWaitingForPath)
	{
		NodeResult = PerformMoveTask2D(OwnerComp, NodeMemory);
	}
	else
	{
		UE_LOG(PathFollowing, Log, TEXT("Pathfinding requests are freezed, waiting..."));
	}

	return NodeResult;

	
	
}

EBTNodeResult::Type UBTTask_MoveTo2D::AbortTask(class UBehaviorTreeComponent * OwnerComp, uint8* NodeMemory)
{
	FBTMoveTo2DMemory* MyMemory = (FBTMoveTo2DMemory*)NodeMemory;
	AAICharacter2DController* MyController = OwnerComp ? Cast<AAICharacter2DController>(OwnerComp->GetOwner()) : NULL;

	if (MyController && MyController->PathFollowingComponent)
	{
		MyController->PathFollowingComponent->AbortMove(TEXT("BehaviorTree abort"), MyMemory->MoveRequestID);
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveTo2D::MoveTo(FVector pos)
{
	
}

FString UBTTask_MoveTo2D::GetStaticDescription() const
{
	FString KeyDesc("invalid");

	return FString::Printf(TEXT("%s:  %s"), *Super::GetStaticDescription(), *KeyDesc);
}

void UBTTask_MoveTo2D::DescribeRuntimeValues(const class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
	const UBlackboardComponent* BlackboardComp = OwnerComp->GetBlackboardComponent();
	FBTMoveTo2DMemory* MyMemory = (FBTMoveTo2DMemory*)NodeMemory;

	if (MyMemory->MoveRequestID && BlackboardComp)
	{
		FString KeyValue = BlackboardComp->DescribeKeyValue(BlackboardKey.GetSelectedKeyID(), EBlackboardDescription::OnlyValue);
		Values.Add(FString::Printf(TEXT("move target: %s"), *KeyValue));
	}
}

uint16 UBTTask_MoveTo2D::GetInstanceMemorySize() const
{
	return sizeof(FBTMoveTo2DMemory);
}

EBTNodeResult::Type UBTTask_MoveTo2D::PerformMoveTask2D(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp->GetBlackboardComponent();

	FBTMoveTo2DMemory* MyMemory = (FBTMoveTo2DMemory*)NodeMemory;
	AAICharacter2DController* MyController = OwnerComp ? Cast<AAICharacter2DController>(OwnerComp->GetOwner()) : NULL;

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	if (MyController && MyBlackboard)
	{
		UCapsuleComponent* MoveComp = MyController->CapsuleComp;
		EPathFollowingRequestResult::Type RequestResult = EPathFollowingRequestResult::Failed;

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			const FVector TargetLocation = MyBlackboard->GetValueAsVector(BlackboardKey.GetSelectedKeyID());

			//UE_LOG(MyLog, Log, TEXT("Move To Task: Target Location: %s"), *TargetLocation.ToString());
			//RequestResult = MyController->MoveToLocation(TargetLocation /*+ FVector(0, 1.0f, 0)*/, AcceptableRadius, true, false, false, true);
			
			RequestResult = MyController->MoveToLocation2D(TargetLocation, AcceptableRadius);
		}

		if (RequestResult == EPathFollowingRequestResult::RequestSuccessful)
		{
			/*const FAIRequestID RequestID = FAIRequestID::CurrentRequest;

			MyMemory->MoveRequestID = RequestID;
			WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished, RequestID);*/

			NodeResult = EBTNodeResult::InProgress;

		}
		else if (RequestResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			NodeResult = EBTNodeResult::Succeeded;
		}

	}



	return NodeResult;
}

#if WITH_EDITOR

FName UBTTask_MoveTo2D::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Task.MoveTo.Icon");
}
#endif