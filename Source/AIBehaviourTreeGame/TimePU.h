// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUp.h"
#include "TimePU.generated.h"

/**
 * 
 */
UCLASS()
class AIBEHAVIOURTREEGAME_API ATimePU : public APowerUp
{
	GENERATED_BODY()
public:
	ATimePU();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration;
	float CurrTimer = 0;
};
