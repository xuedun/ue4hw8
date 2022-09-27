// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIControllerBot.h"
#include "MyAIControllerProtectee.h"
#include "MyCharacterBot.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameModeBase.h"
#include "EMyEnum.h"

AMyAIControllerBot::AMyAIControllerBot()
{

}

void AMyAIControllerBot::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AMyCharacterBot* Bot = Cast<AMyCharacterBot>(InPawn);
	if (Bot && Bot->BotBehaviorTree)
	{
		if (Bot->BotBehaviorTree->BlackboardAsset)
		{
			BlackboardComponent->InitializeBlackboard(*Bot->BotBehaviorTree->BlackboardAsset);
		}
		TargetID = BlackboardComponent->GetKeyID("Target");
		ProID = BlackboardComponent->GetKeyID("Protectee");
		BehaviorTreeComponent->StartTree(*Bot->BotBehaviorTree);
	}
}

void AMyAIControllerBot::OnUnPossess()
{
	Super::OnUnPossess();
	BehaviorTreeComponent->StopTree();
}

void AMyAIControllerBot::PVEDeath(AActor* DamageCauser)
{

	GetPawn()->Destroy();
}

ACharacterBase* AMyAIControllerBot::GetTarget()
{
	if (BlackboardComponent)
	{
		return Cast<ACharacterBase>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(TargetID));
	}
	return nullptr;
}

void AMyAIControllerBot::SetTarget(APawn* InPawn)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(TargetID, InPawn);
		SetFocus(InPawn);
	}
}

//初始化锁定守护者作为目标
void AMyAIControllerBot::Init()
{
	AMyCharacterBot* Bot = Cast<AMyCharacterBot>(GetPawn());
	if (Bot && Bot->Is_Alive())
	{
		//初始化目标定位守护者
		AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		ACharacterBase* Target = nullptr;
		if(GameMode->Protectee)
			Target =Cast<ACharacterBase>( GameMode->Protectee->GetPawn());
		else
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("守护者未创建无法定位目标")));
		}
		if (Target)
		{
			if (BlackboardComponent)
			{
				BlackboardComponent->SetValue<UBlackboardKeyType_Object>(ProID, Target);
			}
			bProTarget = true;
			SetTarget(Target);
		}
	}
}

void AMyAIControllerBot::DistanceDetect()
{
	if (ACharacterBase* Cha = GetTarget())
	{
		if (FVector::Distance(Cha->GetActorLocation(), GetPawn()->GetActorLocation()) > MaxTargetDistance)
			Init();
	}
}

//
void AMyAIControllerBot::ShootEnemyBoss()
{	
//	UKismetSystemLibrary::PrintString(GetWorld(), UKismetSystemLibrary::GetObjectName(GetTarget()));
	//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("bProTarget:%i"), bProTarget));
	AMyCharacterBot* Bot = Cast<AMyCharacterBot>(GetPawn());
	if (Bot && Bot->Is_Alive())
	{
		ACharacterBase* Target = GetTarget();
		if (Target && Target->Is_Alive())
		{
			SetFocus(Target);
			if (LineOfSightTo(Target, Bot->GetActorLocation()))
			{
				float dis = FVector::Distance(Target->GetActorLocation(), Bot->GetActorLocation());
				if (dis < 100)
				{
					Bot->ThreeHookHit();
				}
				else
				{
					Bot->HadokenAttack();
				}
			}
		}
		else
		{
			if (BlackboardComponent)
			{
				Target = Cast<ACharacterBase>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(ProID));
				SetTarget(Target);
				bProTarget = true;
			}
		}
	}
}

//在拥有目标且能看到目标的时候进行攻击
void AMyAIControllerBot::ShootEnemy()
{
//	UKismetSystemLibrary::PrintString(GetWorld(), UKismetSystemLibrary::GetObjectName(GetTarget()));
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("bProTarget:%i"), bProTarget));
	AMyCharacterBot* Bot = Cast<AMyCharacterBot>(GetPawn());
	if (Bot && Bot->Is_Alive())
	{
		ACharacterBase* Target = GetTarget();
		if (Target && Target->Is_Alive())
		{
			if (LineOfSightTo(Target, Bot->GetActorLocation()))
			{

				AWeaponBase* Weapon = Bot->GetCurrentWeapon();
				if (Bot->BotType != AIType::Hook && !Weapon)
				{
					Bot->ChangeToNextWeapon();
				}
				Bot->InputFirePressed();
				Bot->InputFireReleased();
			}
		}
		else
		{
			if (BlackboardComponent)
			{
				Target = Cast<ACharacterBase>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(ProID));
				SetTarget(Target);
				bProTarget = true;
			}
		}
	}
}


