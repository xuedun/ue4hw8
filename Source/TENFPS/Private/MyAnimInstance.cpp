// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "CharacterBase.h"

void UMyAnimInstance::AnimNotify_GrabWeaponR(UAnimNotify* Notify)
{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AnimNotify_GrabWeaponR"));
	Owner->GrabWeaponR();
}

void UMyAnimInstance::AnimNotify_OnHolster(UAnimNotify* Notify)
{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AnimNotify_OnHolster"));
	Owner->OnHolster();
}

void UMyAnimInstance::AnimNotify_GrabWeaponLTemp(UAnimNotify* Notify)
{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AnimNotify_GrabWeaponLTemp"));
	Owner->GrabWeaponLTemp();
}

void UMyAnimInstance::GetEssentialValue()
{
	bADS = Owner->bADS;
	bCombatReady = Owner->bCombatReady;
	Pitch = Owner->Pitch;
	Yaw = Owner->Yaw;
	WeaponType = Owner->CurrentWeaponType;
//	AimBaselineOffset = Owner->AimBaseLineOffset[Owner->CurrentWeaponIndex];
}
//
//void UMyAnimInstance::GetValue_Implementation()
//{
//
//	bADS = Owner->bADS;
//	bCombatReady = Owner->bCombatReady;
//	Pitch = Owner->Pitch;
//	Yaw = Owner->Yaw;
//	WeaponType = Owner->CurrentWeaponType;
//}
//
//bool UMyAnimInstance::GetValue_Validate()
//{
//	return true;
//}

