

#include "Scroller2D.h"
#include "AICharacter2DController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"


AAICharacter2DController::AAICharacter2DController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//static ConstructorHelpers::FObjectFinder<UMovementComponent> Movement(TEXT(""))


	BlackboardComp = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	BehaviorComp = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));


	bUseCollision = false;

	bWantsPlayerState = true;

	bIsPathComplete = false;

	PrimaryActorTick.bCanEverTick = true;

	bShowPathFinding = false;

	PathColor = FColor::Red;

	bUseSteering = false;

	CapsuleComp = NULL;

	MaxLookAhead = 0.0f;

	timeToNextNode = 2.0f;
	ArrivalDistance = 0.0f;
}

void AAICharacter2DController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);
	
	AAICharacter2D * Bot = Cast<AAICharacter2D>(InPawn);
	
	if (Bot && Bot->BotBehavior)
	{
		BlackboardComp->InitializeBlackboard(Bot->BotBehavior->BlackboardAsset);

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		EnemyLocationID = BlackboardComp->GetKeyID("EnemyPosition");

		PathNodeID = BlackboardComp->GetKeyID("PathNode");

		BehaviorComp->StartTree(Bot->BotBehavior);
		CapsuleComp = Bot->CapsuleComponent;
		if (Bot->bUseSteering)
		{
			bUseSteering = true;
			MaxLookAhead = Bot->MaxLookAhead;
			MinCollisionRadius = Bot->MinCollisionRadius;
		}
		bUseCollision = Bot->bUseCollisionAvoidance;
		ArrivalDistance = Bot->ArrivalDistance;
		bUseArrival = Bot->bUseArrival;
	}


}

void AAICharacter2DController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AAICharacter2D* bot = NULL;
	bot = Cast<AAICharacter2D>(GetPawn());

	if (Cast<AAICharacter2D>(GetPawn())->bShowPathFinding || bShowPathFinding)
	{
		DrawPath();
	}

	if (CapsuleComp && bot && bot->bUseSteering)
	{
		CapsuleComp->AddForce(steering);
		maxAvoidanceForce = bot->MaxAvoidanceForce;
		bot->GetController()->SetControlRotation(FRotator(0.f, 0.f, 0.f));

	}

	timeToNextNode -= DeltaSeconds;
	

}

void AAICharacter2DController::BeginInactiveState()
{
	Super::BeginInactiveState();

	AGameState* GameState = GetWorld()->GameState;

	const float MinRespawnDelay = (GameState && GameState->GameModeClass) ? GetDefault<AGameMode>(GameState->GameModeClass)->MinRespawnDelay : 1.0f;

	GetWorldTimerManager().SetTimer(this, &AAICharacter2DController::Respawn, MinRespawnDelay);
}

void AAICharacter2DController::Respawn()
{
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void AAICharacter2DController::SearchForEnemy()
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;

	ACharacter2D* BestPawn = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ACharacter2D* TestPawn = Cast<ACharacter2D>(*It);
		if (TestPawn)
		{
			const float DistSq = FVector::Dist(TestPawn->GetActorLocation(), MyLoc);
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}
	}

	if (BestPawn)
	{
		SetEnemy(BestPawn);
	}
	else
	{
		UE_LOG(MyLog, Warning, TEXT("Enemy not found!!!!"));
	}

	//move this to tick in the future
	for (FActorIterator It = FActorIterator::FActorIterator(GetWorld()); It; ++It)
	{
		ATileset2D* TestNav = Cast<ATileset2D>(*It);
		if (TestNav)
		{
			//UE_LOG(MyLog, Log, TEXT("Navigation name is:  %s"), *TestNav->GetName());
			Navigation = TestNav;
		}
	}
}

bool AAICharacter2DController::SearchForEnemyWithLOS(ACharacter2D* ExcludeEnemy)
{

	bool bGotEnemy = false;
	APawn* MyBot = GetPawn();

	if (MyBot != NULL)
	{
		const FVector MyLoc = MyBot->GetActorLocation();
		float BestDistSq = MAX_FLT;
		ACharacter2D* BestPawn = NULL;

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter2D* TestPawn = Cast<ACharacter2D>(*It);
			if (TestPawn && TestPawn != ExcludeEnemy)
			{
				const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
				if (DistSq < BestDistSq)
				{
					BestDistSq = DistSq;
					BestPawn = TestPawn;
				}
			}
		}
		if (BestPawn)
		{
			SetEnemy(BestPawn);
			bGotEnemy = true;
		}
	}

	return bGotEnemy;
}

void AAICharacter2DController::SetEnemy(class APawn * InPawn)
{
	BlackboardComp->SetValueAsObject(EnemyKeyID, InPawn);

	BlackboardComp->SetValueAsVector(EnemyLocationID, InPawn->GetActorLocation());


	
}

void AAICharacter2DController::FollowPath()
{
	APawn* bot = GetPawn();
	APawn* enemy = Cast<APawn>(BlackboardComp->GetValueAsObject(EnemyKeyID));
	AAICharacter2D* MyBot = Cast<AAICharacter2D>(bot);

	//UE_LOG(MyLog, Log, TEXT("*********************FOLLOW PATH START %s**********************"), *MyBot->GetName());

	FVector currentNode = BlackboardComp->GetValueAsVector(PathNodeID);

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ACharacter2D* TestPawn = Cast<ACharacter2D>(*It);
		if (TestPawn)
		{
			enemy = TestPawn;
			break;
		}
		

	}

	if (enemy)
	{
		FVector AIPos = bot->GetActorLocation();
		Navigation->Grid.findClosestNode(AIPos, Navigation->Offset);
		FVector nextNode = AIPos;

		//UE_LOG(MyLog, Log, TEXT("FOLLOW PATH: THERE IS AN ENEMY!!!"));
		if (path.Num() > 0)
		{
			//UE_LOG(MyLog, Log, TEXT("FOLLOW PATH: THERE IS A PATH ALREADY!!!"));
			//UE_LOG(MyLog, Log, TEXT("Current node is:  %s, Next node is:  %s"), *nextNode.ToString(), *path[path.Num() - 1].ToString());

			int32 index = path.Num() - 1;
			nextNode = path[index];

			
			//if the enemy has moved from the target node then repath
			if (FVector::Dist(path[0], enemy->GetActorLocation()) > Navigation->Offset)
			{
				
				//UE_LOG(MyLog, Log, TEXT("Enemy has moved!!!"));
				if (!Navigation->bPathfindingInUse && SetPath(enemy->GetActorLocation()))
				{
					//UE_LOG(MyLog, Log, TEXT("FOLLOW PATH: PATH FOUND!!!"));

					bIsPathComplete = false;
					index = path.Num() - 1;
					nextNode = path[index];
					path.Remove(nextNode);

				}
				else
				{
					//UE_LOG(MyLog, Log, TEXT("No Path set "))
					nextNode = enemy->GetActorLocation();
					bIsPathComplete = true;
				}
			}

			else if (FVector::Dist(bot->GetActorLocation(), nextNode) < (Navigation->Offset))
			{
				//UE_LOG(MyLog, Log, TEXT("Move to next node in path"));
			
				path.Remove(nextNode);

			}
		}

		else
		{
			if (SetPath(enemy->GetActorLocation()) )
			{
				if (path.Num() > 0)
				{
					//UE_LOG(MyLog, Log, TEXT("FOLLOW PATH: PATH FOUND 2!!!"));

					int32 index = path.Num() - 1;
					nextNode = path[index];
					path.Remove(nextNode);
				}
			}
			else
			{
				UE_LOG(MyLog, Log, TEXT("PATH NOT FOUND!!!"));
			}
		}
		BlackboardComp->SetValueAsVector(PathNodeID, nextNode + FVector(0, 0, -Navigation->Offset/2));
		//MoveToLocation(nextNode, true, false, false, true);

	}
	
	else
	{
		UE_LOG(MyLog, Log, TEXT("NO ENEMY FOUND!!!"));
	}

	
	//UE_LOG(MyLog, Log, TEXT("*********************FOLLOW PATH END %s************************"), *MyBot->GetName());

}

EPathFollowingRequestResult::Type AAICharacter2DController::FollowPath2D(const FVector &Dest, float AcceptanceRadius)
{
	APawn* bot = GetPawn();
	AAICharacter2D* MyBot = Cast<AAICharacter2D>(bot);
	EPathFollowingRequestResult::Type Result = EPathFollowingRequestResult::Failed;

	UE_LOG(MyLog, Log, TEXT("*********************FOLLOW PATH 2D START %s**********************"), *MyBot->GetName());


	//int32 numNodes = path.Num();

	

	FVector currentNode = BlackboardComp->GetValueAsVector(PathNodeID);


	if ( Navigation)
	{
		FVector AIPos = bot->GetActorLocation();
		Navigation->Grid.findClosestNode(AIPos, Navigation->Offset);
		FVector nextNode = AIPos;

		//UE_LOG(PathFollowing, Log, TEXT("FOLLOW PATH: THERE IS AN ENEMY!!!"));
		if (path.Num() > 0)
		{
			UE_LOG(PathFollowing, Log, TEXT("FOLLOW PATH: THERE IS A PATH ALREADY!!!"));
			UE_LOG(PathFollowing, Log, TEXT("Current node is:  %s, Next node is:  %s"), *nextNode.ToString(), *path[path.Num() - 1].ToString());

			int32 index = path.Num() - 1;
			nextNode = path[index];


			//if the enemy has moved from the target node then repath
			if (FVector::Dist(path[0], Dest) > Navigation->Offset)
			{

				UE_LOG(PathFollowing, Log, TEXT("Enemy has moved!!!"));
				if (!Navigation->bPathfindingInUse && SetPath(Dest))
				{
					//UE_LOG(MyLog, Log, TEXT("FOLLOW PATH: PATH FOUND!!!"));

					Result = EPathFollowingRequestResult::RequestSuccessful;
					bIsPathComplete = false;
					index = path.Num() - 1;
					nextNode = path[index];
					path.Remove(nextNode);

				}
				else
				{
					//UE_LOG(MyLog, Log, TEXT("No Path set "))
					nextNode = Dest;
					bIsPathComplete = true;
				}
			}
			//bot has reached next path node switch to path point and remove previous point
			else if (FVector::Dist(bot->GetActorLocation(), nextNode) < (Navigation->Offset))
			{
				UE_LOG(PathFollowing, Log, TEXT("Move to next node in path"));
				Result = EPathFollowingRequestResult::RequestSuccessful;
				path.Remove(nextNode);

			}
			//if the bot overshoots the next node then repath to see if there is a better path from their
			//current location
			else if (FVector::Dist(bot->GetActorLocation(), nextNode) > (Navigation->Offset * 2) || timeToNextNode <= 0)
			{
				if (!Navigation->bPathfindingInUse && SetPath(Dest))
				{
					Result = EPathFollowingRequestResult::RequestSuccessful;
					bIsPathComplete = false;
					index = path.Num() - 1;
					nextNode = path[index];
					path.Remove(nextNode);
					
				}
				timeToNextNode = 2.f;
			}
			else
			{
				Result = EPathFollowingRequestResult::RequestSuccessful;
			}
		}

		else
		{
			if (SetPath(Dest))
			{
				if (path.Num() > 0)
				{
					//UE_LOG(PathFollowing, Log, TEXT("FOLLOW PATH: PATH FOUND 2!!!"));

					Result = EPathFollowingRequestResult::RequestSuccessful;
					int32 index = path.Num() - 1;
					nextNode = path[index];
					path.Remove(nextNode);
				}
			}
			else
			{
				UE_LOG(PathFollowing, Log, TEXT("PATH NOT FOUND!!!"));
				
			}
		}



		BlackboardComp->SetValueAsVector(PathNodeID, nextNode /*+ FVector(0, 0, -Navigation->Offset / 2)*/);
		UE_LOG(PathFollowing, Log, TEXT("AI Position:  %s,  Next node:  %s"), *bot->GetActorLocation().ToString(), *BlackboardComp->GetValueAsVector(PathNodeID).ToString());

		//Result = MoveToLocation2D(nextNode, 0.0f);
		if (HasReached2D(Dest, AcceptanceRadius))
		{
			Result = EPathFollowingRequestResult::AlreadyAtGoal;
		}
	}

	else
	{
		UE_LOG(PathFollowing, Log, TEXT("NO ENEMY FOUND!!!"));
	}
	return Result;

	
	UE_LOG(MyLog, Log, TEXT("*********************FOLLOW PATH 2d END %s************************"), *MyBot->GetName());
}

uint8 AAICharacter2DController::GetPathKeyID()
{
	return PathNodeID;
}

bool AAICharacter2DController::SetPath(FVector end)
{
	bool pathFound = false;

	APawn* bot = GetPawn();

	if (bot && Navigation)
	{
		if (FVector::Dist(bot->GetActorLocation(), end) < (Navigation->Offset))
		{
			UE_LOG(PathFollowing, Log, TEXT("SET PATH: ENEMY TOO CLOSE!!!"));

			path.Reset();

			path.Add(end);
			pathFound = true;

		}

		else
		{
			path.Reset();
			if (!Navigation->bPathfindingInUse)
			{
				//UE_LOG(PathFollowing, Log, TEXT("SET PATH:  AI LOCATION:  %s, DESTINATION:  %s"), *bot->GetActorLocation().ToString(), *end.ToString())
				Navigation->FindPath(bot->GetActorLocation(), end, path);

				pathFound = true;
			}
			else
			{
				pathFound = false;
			}
		}
	}
	else if (!bot)
	{
		UE_LOG(PathFollowing, Warning, TEXT("SET PATH: BOT NOT FOUND!!!"));
	}
	else if (!Navigation)
	{
		UE_LOG(PathFollowing, Warning, TEXT("SET PATH:  NAVIGATION NOT FOUND!!!"));
	}
	
	return pathFound;
}

TArray<FVector> AAICharacter2DController::GetPath()
{
	return path;
}

void AAICharacter2DController::DrawPath()
{
	AAICharacter2D* me = Cast<AAICharacter2D>(GetPawn());


	if (me)
	{
		PathColor = me->PathColor;
	}

	for (int32 i = path.Num() - 1; i > 0; i--)
	{
		DrawDebugLine(GetWorld(), path[i] + FVector(0, 1, 0), path[i - 1] + FVector(0, 1, 0), PathColor, false, 0.3f, 0.03, 1.0f);
		//DrawDebugSphere(GetWorld(), GetPawn()->GetActorLocation(), 32, 32, FColor::Red, false, 0.2);
	}
}

EPathFollowingRequestResult::Type AAICharacter2DController::MoveToLocation2D(const FVector& Dest, float AcceptanceRadius)
{
	EPathFollowingRequestResult::Type Result = EPathFollowingRequestResult::Failed;
	AAICharacter2D* MyPawn = Cast<AAICharacter2D>(GetPawn());

	bool bCanRequestMove = true;

	FVector GoalLocation = Dest;

	FVector NextNode = FVector::ZeroVector;

	AvoidanceForce = FVector::ZeroVector;
	

	//check for collision with other bots and calculate necessary steering force to avoid object
	

	//check to see if the bot has reached the goal node 
	if (bCanRequestMove && HasReached2D(GoalLocation, AcceptanceRadius))
	{
		//UE_LOG(PathFollowing, Log, TEXT("MoveToLocation2D:  already at goal!"));

		OnMoveCompleted(FAIRequestID::CurrentRequest, EPathFollowingResult::Success);
		Result = EPathFollowingRequestResult::AlreadyAtGoal;
		bCanRequestMove = false;

	}

	if (bCanRequestMove)
	{
		Result = FollowPath2D(Dest, AcceptanceRadius);
		
		//UE_LOG(PathFollowing, Log, TEXT("Result is:  %d"), (int32)Result);
		

		if (Result == EPathFollowingRequestResult::RequestSuccessful || Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			if (MyPawn && CapsuleComp)
			{
				NextNode = BlackboardComp->GetValueAsVector(PathNodeID);
								
				//current velocity of bot
				FVector Velocity = CapsuleComp->GetComponentVelocity();

				if (bUseCollision)
				{
					Collision(MyPawn);
				}

				if (MyPawn->bUsePursuit)
				{
					Pursuit(MyPawn);
				}
				//force applied to steer pawn to next node in the path
				Force = NextNode - MyPawn->GetActorLocation();
				steering = FVector::ZeroVector;
				

				//if the distance to target is less than ArrivalDistance then start to slow down
				if (Force.Normalize())
				{
					//steering /= CapsuleComp->GetMass();
					float distance = FVector::Dist(NextNode, MyPawn->GetActorLocation());

					if (bUseArrival && distance < ArrivalDistance)
					{
						Force *= MyPawn->MaxThrust * (distance / ArrivalDistance);

					}
					else
					{
						Force *= MyPawn->MaxThrust;
					}
					steering = Force - Velocity;

					//steering += Velocity;

				}

				else
				{
					UE_LOG(PathFollowing, Warning, TEXT("Force could not be normalized!!!"));
				}

				//CapsuleComp->AddForceAtLocation(steering, FVector(0, 0, 0));
				//UE_LOG(PathFollowing, Log, TEXT("Steering:  %s"), *steering.ToString());
				
				steering += AvoidanceForce;
			}
			
		}
		else if (Result == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(PathFollowing, Log, TEXT("Path request failed!!!"));

		}
	}

	if (Result == EPathFollowingRequestResult::Failed)
	{
		OnMoveCompleted(FAIRequestID::InvalidRequest, EPathFollowingResult::Invalid);
	}

	return Result;
}

bool AAICharacter2DController::HasReached2D(FVector TestPoint, float AcceptanceRadius)
{
	const FVector CurrentLocation = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;

	if (FVector::Dist(TestPoint, CurrentLocation) < AcceptanceRadius)
	{
		return true;
	}
	return false;
}

FVector AAICharacter2DController::seek()
{
	return FVector::ZeroVector;
}

void AAICharacter2DController::Collision(AAICharacter2D* MyPawn)
{
	if (CapsuleComp && MyPawn)
	{
		AAICharacter2D * CollidingPawn = NULL;
		float distance = 0.f;
		FVector ahead;
		FVector ahead2;

		FVector velocity = CapsuleComp->GetComponentVelocity();
		if (velocity.Normalize())
		{
			//decide which ai bot to calculate collision avoidance with
			for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
			{
				AAICharacter2D * TestPawn = Cast<AAICharacter2D>(*It);

				if (TestPawn && MyPawn != TestPawn)
				{
					ahead = MyPawn->GetActorLocation() + velocity * MaxLookAhead;
					ahead2 = MyPawn->GetActorLocation() + velocity * MaxLookAhead * 0.5;

					//collision detected
					if (FVector::Dist(ahead, TestPawn->GetActorLocation()) < MinCollisionRadius || FVector::Dist(ahead2, TestPawn->GetActorLocation()) < MinCollisionRadius)
					{
						if (CollidingPawn == NULL)
						{
							CollidingPawn = TestPawn;
							distance = FVector::Dist(CollidingPawn->GetActorLocation(), MyPawn->GetActorLocation());
						}

						if (distance > FVector::Dist(TestPawn->GetActorLocation(), MyPawn->GetActorLocation()))
						{
							CollidingPawn = TestPawn;
						}
					}
				}
			}

			//if a collision is detected, then calculate avoidance force to be added
			if (CollidingPawn)
			{
				AvoidanceForce = ahead - CollidingPawn->GetActorLocation();
				if (AvoidanceForce.Normalize())
				{
					AvoidanceForce *= maxAvoidanceForce;
				}
			}
		}
	}
}

void AAICharacter2DController::Pursuit(AAICharacter2D* MyPawn)
{
	if (CapsuleComp && MyPawn->IsValidLowLevel())
	{
		
	}
}

bool AAICharacter2DController::LineOfSight()
{
	return true;
}

FVector AAICharacter2DController::queue()
{
	return FVector::ZeroVector;
}