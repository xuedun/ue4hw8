// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "MyCharacterBot.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"
#include "EMyEnum.h"

AMyAIController::AMyAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));

	EnemyKeyID = 0;
}

void AMyAIController::FindTarget()
{
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor Name")));
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
}

void AMyAIController::ShootEnemy()
{

}

void AMyAIController::PVEDeath(AActor* DamageCauser)
{
	this->OnUnPossess();
	GetPawn()->Destroy();
}


ACharacterBase* AMyAIController::GetTarget()
{
	if (BlackboardComponent)
	{
		return Cast<ACharacterBase>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}
	return nullptr;
}



bool AMyAIController::HasTarget(AActor* Target)
{
	static FName Tag = FName(TEXT("HasTarget"));
	FCollisionQueryParams Params(Tag, true, GetPawn());
	Params.bReturnPhysicalMaterial = true;

	FVector Start = GetPawn()->GetActorLocation();
	Start.Z += GetPawn()->BaseEyeHeight;
	const FVector End = Target->GetActorLocation();

	FHitResult OutHit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, Params);
	if (OutHit.bBlockingHit)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			if (HitActor == Target)
			{
				return true;
			}
		}
	}
	return false;
}

void AMyAIController::SetTarget(APawn* InPawn)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

