// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Controller.h"
#include "MyPlayerController.h"
#include "CharacterBase.h"

void UAttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Health")));
	Player = MeshComp->GetOwner();
	if (Player)
	{
		Location = MeshComp->GetSocketLocation("index_01_r");
	}
}

void UAttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Tick")));
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), MeshComp->GetSocketLocation("hand_r"), MeshComp->GetSocketLocation("index_01_r"), 20, ETraceTypeQuery::TraceTypeQuery1, false,
		ActorsToIgnore, EDrawDebugTrace::Persistent, HitResults, true);
	for(int i =0;i<HitResults.Num();i++)
	{
		AActor* HitActor = HitResults[i].GetActor();
		if (!HitActors.Contains(HitActor))
		{
			HitActors.Add((HitActor));
//			UGameplayStatics::ApplyDamage(HitActor,10.f,EventInstigator,Player,)
		}
	}
	Location = MeshComp->GetSocketLocation("index_01_r");

}

void UAttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HitActors.Empty();
}
