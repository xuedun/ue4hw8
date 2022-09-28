// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_RunAway.h"
#include "MyBTTask_GetPosition.h"
#include "MyAIControllerBot.h"
#include "CharacterBase.h"
#include "MyCharacterBot.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

EBTNodeResult::Type UMyBTTask_RunAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyAIControllerBot* AIC = Cast<AMyAIControllerBot>(OwnerComp.GetAIOwner());
	if (AIC)
	{
		AMyCharacterBot* Bot = Cast<AMyCharacterBot>(AIC->GetPawn());
		ACharacterBase* Enemy = AIC->GetTarget();
		if (Bot && Enemy && AIC->IsHurt())
		{
			FVector Origin = Enemy->GetActorLocation();
			FVector Now = Bot->GetActorLocation();

			FVector RunAwayLocation = 2 * Now - Origin;

			AIC->BeHurt(false);
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), RunAwayLocation);
			
// 			float Radius = 800.0f;
// 			if (Bot->BotType == AIType::Hook || Bot->Tags.Contains(TEXT("Boss"))) Radius = 50.0f;
// 			UNavigationSystemV1* NavMesh = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIC);
// 			if (NavMesh)
// 			{
// 				FNavLocation Loc;
// 				NavMesh->GetRandomReachablePointInRadius(Origin, Radius, Loc);
// 				if (Loc.Location != FVector::ZeroVector)
// 				{
// 					OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Loc.Location);
// 					return EBTNodeResult::Succeeded;
// 				}
// 			}

			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
