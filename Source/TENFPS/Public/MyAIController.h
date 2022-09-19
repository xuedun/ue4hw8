// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class ACharacterBase;

UCLASS()
class TENFPS_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyAIController();

	UFUNCTION(BlueprintCallable,Category=Behavior)
		void FindTarget();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		virtual void ShootEnemy();

	UFUNCTION(BlueprintCallable)
		virtual void PVEDeath(AActor* DamageCauser);
	bool HasTarget(AActor* Target);
	void SetTarget(APawn* InPawn);
	ACharacterBase* GetTarget();
	ACharacterBase* Bot;
//	void DestoryPawn();
//	FTimerHandle TimerHandle;
protected:


	UBlackboardComponent* BlackboardComponent;
	UBehaviorTreeComponent* BehaviorTreeComponent;

	int32 EnemyKeyID;
};
