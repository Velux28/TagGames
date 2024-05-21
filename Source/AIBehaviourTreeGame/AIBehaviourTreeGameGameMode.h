// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AIBehaviourTreeGameGameMode.generated.h"

UCLASS(minimalapi)
class AAIBehaviourTreeGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	TArray<class ATargetPoint*> TargetPoints;
	TArray<class ABall*> GameBalls;

	void ResetMatch();

public:
	AAIBehaviourTreeGameGameMode();

	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	const TArray<class ABall*>& GetBalls() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BallAttachedToPlayer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PreviousScore = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GameTimer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PreviousTimer = 0;

	bool bGameStart = false;

};



