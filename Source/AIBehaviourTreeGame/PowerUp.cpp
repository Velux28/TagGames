// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp.h"

// Sets default values
APowerUp::APowerUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PUMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PUMesh"));
	SetRootComponent(PUMesh);
	PUBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PUBox"));
	PUBox->AttachToComponent(PUMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void APowerUp::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void APowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APowerUp::ApplyEffect(AAIBehaviourTreeGameCharacter* Character)
{

}
