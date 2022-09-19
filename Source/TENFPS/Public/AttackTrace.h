// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackTrace.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API UAttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	TArray<FHitResult> HitResults;
	TArray<AActor*> HitActors;
	class AMyPlayerController* EventInstigator;
	TSubclassOf<UDamageType> DamageTypeClass;
	FVector Location;
	TArray<AActor*> ActorsToIgnore;
	AActor* Player;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
