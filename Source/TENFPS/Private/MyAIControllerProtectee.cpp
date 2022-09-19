// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIControllerProtectee.h"

#include "MyCharacterProtectee.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameModeBase.h"
#include "EMyEnum.h"

AMyAIControllerProtectee::AMyAIControllerProtectee()
{
}

void AMyAIControllerProtectee::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AMyCharacterProtectee* MyBot = Cast<AMyCharacterProtectee>(InPawn);
	if (MyBot && MyBot->BotBehaviorTree)
	{
		if (MyBot->BotBehaviorTree->BlackboardAsset)
		{
			BlackboardComponent->InitializeBlackboard(*MyBot->BotBehaviorTree->BlackboardAsset);
		}
		EnemyKeyID = BlackboardComponent->GetKeyID("Enemy");
		BehaviorTreeComponent->StartTree(*MyBot->BotBehaviorTree);
	}
}

void AMyAIControllerProtectee::OnUnPossess()
{
	Super::OnUnPossess();
	BehaviorTreeComponent->StopTree();
}

void AMyAIControllerProtectee::FindTarget()
{
	if (Bot && Bot->Is_Alive())
	{
		ACharacterBase* Target = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor Name : %s"), *Target->GetName()));
		if (Target && Target->Is_Alive())
		{
			if (HasTarget(Target))
			{
				SetTarget(Target);
			}
		}
	}
// 	for (auto PIC : AICs)
// 	{
// 		ACharacterBase* MyBot = Cast<ACharacterBase>(PIC->GetPawn());
// 		if (MyBot && MyBot->Is_Alive())
// 		{
// 			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor Name : %s"), *MyBot->GetName()));
// 			SetTarget(MyBot);
// 			break;
// 		}
// 	}
}

