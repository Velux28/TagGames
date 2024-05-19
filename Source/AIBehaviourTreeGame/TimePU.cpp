// Fill out your copyright notice in the Description page of Project Settings.


#include "TimePU.h"
ATimePU::ATimePU()
{
	
}
void ATimePU::BeginPlay()
{
	Super::BeginPlay();
}
void ATimePU::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive)
	{
		CurrTimer += DeltaTime;
		if (CurrTimer >= Duration)
		{
			CurrTimer = 0;
			bActive = false;
		}
	}
}