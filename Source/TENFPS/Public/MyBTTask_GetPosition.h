// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_GetPosition.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API UMyBTTask_GetPosition : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

};
