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
	bool CanSeeTarget(AActor* Target);
	virtual ACharacterBase* GetTarget();
	int32 TargetID;
protected:
	UBlackboardComponent* BlackboardComponent;
	UBehaviorTreeComponent* BehaviorTreeComponent;
};
