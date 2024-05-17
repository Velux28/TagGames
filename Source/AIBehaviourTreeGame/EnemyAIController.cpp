#include "EnemyAIController.h"
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
			return SearchForBall;
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
				int64 BallValue = FVector::Distance(AIController->GetPawn()->GetActorLocation(), BallsList[i]->GetActorLocation()) - BallsList[i]->SpeedIncreace * RandomFactor;
				if (!BallsList[i]->GetAttachParentActor() &&
					(!NearestBall ||
						BallValue<
						BestBallValue))
				{
					BestBallValue = BallValue;
					NearestBall = BallsList[i];
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("Ball: %d"), BestBallValue);
			BestBall = NearestBall;
			UE_LOG(LogTemp, Warning, TEXT("BestBall"));
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			if (BestBall)
			{
				return GoToBall;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Ricerco la palla"));
				return WaitForBall;
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
				return SearchForBall;
			}

			BestBall->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
			BestBall->SetActorRelativeLocation(FVector(0, 0, 0));

			return GoToPlayer;
		}
	);

	CurrentState = SearchForBall;
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
