// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIBehaviourTreeGameGameMode.h"
#include "AIBehaviourTreeGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Ball.h"
#include "Engine/TargetPoint.h"

AAIBehaviourTreeGameGameMode::AAIBehaviourTreeGameGameMode()
{
	// set default pawn class to our Blueprinted character
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AAIBehaviourTreeGameGameMode::BeginPlay()
{
	Super::BeginPlay();
	ResetMatch();
}

void AAIBehaviourTreeGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGameStart)
	{
		GameTimer += DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("%f sec"), GameTimer);
	}

	for (int32 i = 0; i < GameBalls.Num(); i++)
	{
		if (GameBalls[i]->GetAttachParentActor() != GetWorld()->GetFirstPlayerController()->GetPawn()) 
		{
			return;
		}
	}


	ResetMatch();
}

const TArray<class ABall*>& AAIBehaviourTreeGameGameMode::GetBalls() const
{
	return GameBalls;
}

void AAIBehaviourTreeGameGameMode::ResetMatch()
{
	PreviousScore = BallAttachedToPlayer;
	PreviousTimer = GameTimer;
	bGameStart = false;
	BallAttachedToPlayer = 0;
	GameTimer = 0;

	TargetPoints.Empty();
	GameBalls.Empty();

	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		TargetPoints.Add(*It);
	}

	for (TActorIterator<ABall> It(GetWorld()); It; ++It)
	{
		if (It->GetAttachParentActor())
		{
			It->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			//It->AttachToActor(nullptr,FAttachmentTransformRules::KeepWorldTransform);
		}

		GameBalls.Add(*It);
	}

	TArray<ATargetPoint*> RandomTargetPoints = TargetPoints;

	for (int32 i = 0; i < GameBalls.Num(); i++)
	{
		const int32 RandomTargetIndex = FMath::RandRange(0, RandomTargetPoints.Num() - 1);
		GameBalls[i]->SetActorLocation(TargetPoints[RandomTargetIndex]->GetActorLocation());
		RandomTargetPoints.RemoveAt(RandomTargetIndex);
	}
}