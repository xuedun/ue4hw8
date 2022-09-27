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
		EnemyKeyID = BlackboardComponent->GetKeyID("Target");
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
	AMyCharacterProtectee* Bot = Cast<AMyCharacterProtectee>(GetPawn());
 	if (Bot && Bot->Is_Alive())
 	{
 		ACharacterBase* Target = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("FindTarget Name : %s"), *Target->GetName()));
 		if (Target && Target->Is_Alive())
 		{
			if (CanSeeTarget(Target))
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

void AMyAIControllerProtectee::SetTarget(APawn* InPawn)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

ACharacterBase* AMyAIControllerProtectee::GetTarget()
{
	if (BlackboardComponent)
	{
		return Cast<ACharacterBase>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}
	return nullptr;
}

