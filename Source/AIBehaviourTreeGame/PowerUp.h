// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviourTreeGameCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PowerUp.generated.h"

UCLASS()
class AIBEHAVIOURTREEGAME_API APowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyEffect(AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveEffect(AActor* Actor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PUMesh;
	UPROPERTY(VisibleAnywhere, Blueprintreadonly)
	UBoxComponent* PUBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bActive;
};
