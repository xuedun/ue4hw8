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

	void ShootEnemy();
};
