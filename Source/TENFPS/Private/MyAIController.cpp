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
}

bool AMyAIController::CanSeeTarget(AActor* Target)
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

ACharacterBase* AMyAIController::GetTarget()
{
	if (BlackboardComponent)
	{
		return Cast<ACharacterBase>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(TargetID));
	}
	return nullptr;
}
