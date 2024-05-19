// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimePU.h"
#include "SpeedPU.generated.h"

/**
 * 
 */
UCLASS()
class AIBEHAVIOURTREEGAME_API ASpeedPU : public ATimePU
{
	GENERATED_BODY()
protected:

	virtual void ApplyEffect()override;
};
