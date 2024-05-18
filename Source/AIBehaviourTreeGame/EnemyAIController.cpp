#include "EnemyAIController.h"
#include "AIBehaviourTreeGameCharacter.h"
#include "Misc/OutputDeviceNull.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIBehaviourTreeGameGameMode.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	GoToPlayer = MakeShared<FAivState>(
		[](AAIController* AIController) {
			AIController->MoveToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), 100.0f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}

			if (BestBall)
			{
				BestBall->AttachToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
				BestBall->SetActorRelativeLocation(FVector(0, 0, 0));
				BestBall = nullptr;
			}
			return WaitForBall;
		}
	);

	SearchForBall = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			AGameModeBase* GameMode = AIController->GetWorld()->GetAuthGameMode();
			AAIBehaviourTreeGameGameMode* AIGameMode = Cast<AAIBehaviourTreeGameGameMode>(GameMode);
			const TArray<ABall*>& BallsList = AIGameMode->GetBalls();

			ABall* NearestBall = nullptr;
			const int32 RandomFactor = FMath::RandRange(0, 15) - 5;
			int64 BestBallValue = 999999999999999999;

			for (int32 i = 0; i < BallsList.Num(); i++)
			{
				int64 BallValue = 999999999999999999;
				if (Cast<AAIBehaviourTreeGameCharacter>(AIController->GetPawn())->bSlowEnemy)
				{
					BallValue = FVector::Distance(AIController->GetPawn()->GetActorLocation(), BallsList[i]->GetActorLocation()) + BallsList[i]->SpeedIncreace * RandomFactor;
				}
				else 
				{
					BallValue = FVector::Distance(AIController->GetPawn()->GetActorLocation(), BallsList[i]->GetActorLocation()) - BallsList[i]->SpeedIncreace * RandomFactor;
				}
				if (!BallsList[i]->GetAttachParentActor() &&
					(!NearestBall ||
						BallValue<
						BestBallValue))
				{
					BestBallValue = BallValue;
					NearestBall = BallsList[i];
				}
			}

			//UE_LOG(LogTemp, Warning, TEXT("Ball: %d"), BestBallValue);
			BestBall = NearestBall;
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			if (BestBall)
			{
				return GoToBall;
			}
			else 
			{
				return ChangePatrol;
			}
		}
	);

	WaitForBall = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			BallCurrTimer = 0;
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			BallCurrTimer += DeltaTime;

			if (BallCurrTimer >= BallWaitTime)
			{
				return SearchForBall;
			}
			return nullptr;
		}
	);

	GoToBall = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			AIController->MoveToActor(BestBall, 100.0f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			return GrabBall;
		}
	);

	GrabBall = MakeShared<FAivState>(
		[this](AAIController* AIController)
		{
			if (BestBall->GetAttachParentActor())
			{
				BestBall = nullptr;
			}
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {

			if (!BestBall)
			{
				return Stunned;
			}

			BestBall->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
			BestBall->SetActorRelativeLocation(FVector(0, 0, 0));

			return GoToPlayer;
		}
	);

	ChangePatrol = MakeShared<FAivState>(
		[this](AAIController* AIController)
		{
			AAIBehaviourTreeGameCharacter* Enemy = Cast<AAIBehaviourTreeGameCharacter>(AIController->GetPawn());
			const int32 RandomFactor = FMath::RandRange(0, Enemy->EnemyPatrolPoints.Num() - 1);
			CurrPatrol = Enemy->ChangePatrolPoint(RandomFactor);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {

			if (!CurrPatrol)
			{
				return WaitForPatrol;
			}

			return MoveToPatrol;
		}
	);

	WaitForPatrol = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			PatrolCurrTimer = 0;
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			if (IsPlayerInSightRadius(AIController->GetWorld()->GetFirstPlayerController()->GetPawn()))
			{
				return SearchForBall;
			}

			PatrolCurrTimer += DeltaTime;

			if (PatrolCurrTimer >= PatrolWaitTime)
			{
				return ChangePatrol;
			}
			return nullptr;
		}
	);

	MoveToPatrol = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			AIController->MoveToActor(CurrPatrol, 100.0f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			if (IsPlayerInSightRadius(AIController->GetWorld()->GetFirstPlayerController()->GetPawn()))
			{
				return SearchForBall;
			}
			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}

			return WaitForPatrol;
		}
	);

	Stunned = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			PatrolCurrTimer = 0;
			UE_LOG(LogTemp, Warning, TEXT("Stunned"));
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			PatrolCurrTimer += DeltaTime;
			if (PatrolCurrTimer >= PatrolWaitTime)
			{
				if (FMath::RandRange(0, 100) < 10)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Patrol"));
					return ChangePatrol;
				}
				return SearchForBall;
			}
			UE_LOG(LogTemp, Warning, TEXT("!Girandola"));
			//AIController->GetPawn()->SetActorRotation(FQuat(AIController->GetPawn()->GetActorRotation().)//AIController->GetPawn()->GetActorRotation() * (1 *DeltaTime));
			return nullptr;
		}
	);

	CurrentState = ChangePatrol;
	CurrentState->CallEnter(this);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		CurrentState = CurrentState->CallTick(this, DeltaTime);
	}
}


bool AEnemyAIController::IsPlayerInSightRadius(APawn* Player)
{
	float SightRadius = Cast<AAIBehaviourTreeGameCharacter>(GetPawn())->SightRadius;
	float SightAngle = Cast<AAIBehaviourTreeGameCharacter>(GetPawn())->SightAngle;
	float PlayerDistance = FVector::Distance(GetPawn()->GetActorLocation(), Player->GetActorLocation());

	if (PlayerDistance <= SightRadius)
	{
		return true;
	}

	return false;
}
