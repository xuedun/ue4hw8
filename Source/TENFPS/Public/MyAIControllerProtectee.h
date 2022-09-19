// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAIController.h"
#include "MyPlayerController.h"
#include "MyAIControllerProtectee.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyAIControllerProtectee : public AMyAIController
{
	GENERATED_BODY()
public:
	AMyAIControllerProtectee();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void FindTarget();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AMyPlayerController*> AICs ;
};
