// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_GetPosition.h"
#include "MyAIController.h"
#include "CharacterBase.h"
#include "MyCharacterBot.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

EBTNodeResult::Type UMyBTTask_GetPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyAIController* AIC = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (AIC)
	{
		AMyCharacterBot* Bot = Cast<AMyCharacterBot>(AIC->GetPawn());
		ACharacterBase* Enemy = AIC->GetTarget();
		if (Bot && Enemy)
		{
			const FVector Origin = Enemy->GetActorLocation();
			float Radius = 800.0f;
			if (Bot->BotType == AIType::Hook||Bot->Tags.Contains(TEXT("Boss"))) Radius = 50.0f;
			UNavigationSystemV1* NavMesh = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIC);
			if (NavMesh)
			{
				FNavLocation Loc;
				NavMesh->GetRandomReachablePointInRadius(Origin, Radius, Loc);
				if (Loc.Location != FVector::ZeroVector)
				{
					OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Loc.Location);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
