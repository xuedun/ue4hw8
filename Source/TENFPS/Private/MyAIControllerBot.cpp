// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIControllerBot.h"
#include "MyCharacterBot.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"
#include "EMyEnum.h"

AMyAIControllerBot::AMyAIControllerBot()
{

}

void AMyAIControllerBot::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AMyCharacterBot* MyBot = Cast<AMyCharacterBot>(InPawn);
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

void AMyAIControllerBot::OnUnPossess()
{
	Super::OnUnPossess();
	BehaviorTreeComponent->StopTree();
}




void AMyAIControllerBot::ShootEnemy()
{
	AMyCharacterBot* MyBot = Cast<AMyCharacterBot>(GetPawn());
	if (MyBot && MyBot->Is_Alive())
	{
		bool bCanFire = false;
		ACharacterBase* Enemy = GetTarget();
		if (Enemy && Enemy->Is_Alive())
		{
			if (LineOfSightTo(Enemy, MyBot->GetActorLocation()))
			{
				AWeaponBase* Weapon = MyBot->GetCurrentWeapon();
				if (MyBot->BotType != AIType::Hook && !Weapon)
				{
					MyBot->ChangeToNextWeapon();
				}
				MyBot->InputFirePressed();
			}
		}
	}
}