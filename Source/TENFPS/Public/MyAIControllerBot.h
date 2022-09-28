// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAIController.h"
#include "MyAIControllerBot.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyAIControllerBot : public AMyAIController
{
	GENERATED_BODY()
public:
	AMyAIControllerBot();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	int32 ProID;//守护者目标
	int32 HurtID;
	bool bProTarget = true;
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ShootEnemy();
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ShootEnemyBoss();
	UFUNCTION(BlueprintCallable)
		void PVEDeath(AActor* DamageCauser);
	bool IsHurt();
	void BeHurt(bool InBool);
	void SetTarget(APawn* InPawn);
	ACharacterBase* GetTarget();
	UPROPERTY(EditAnywhere)
		float MaxTargetDistance = 1000;
	UFUNCTION(BlueprintCallable)
		void Init();
	UFUNCTION(BlueprintCallable)
		void DistanceDetect();
};
