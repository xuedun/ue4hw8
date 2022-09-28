// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyDamageType_Dot.h"
#include "CharacterBase.h"


void AMyPlayerController::UpdateDotDamage()
{
	if (!Chara) Chara = Cast<ACharacterBase>(GetPawn());
//	UGameplayStatics::ApplyDamage(Chara, Chara->DamagePerSec, nullptr, nullptr, UMyDamageType_Dot::StaticClass());
	UGameplayStatics::ApplyDamage(Chara, 10, nullptr, nullptr, UMyDamageType_Dot::StaticClass());
}

void AMyPlayerController::PlayerCameraShake(TSubclassOf<UCameraShakeBase> CameraShake)
{
	ClientPlayCameraShake(CameraShake, 1, ECameraShakePlaySpace::CameraLocal,FRotator::ZeroRotator);
}

