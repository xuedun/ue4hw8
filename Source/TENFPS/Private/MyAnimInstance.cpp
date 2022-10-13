// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "CharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"
void UMyAnimInstance::AnimNotify_GrabWeaponR(UAnimNotify* Notify)
{
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("AnimNotify_GrabWeaponR")));
//	if(!Owner) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("ERROR"));
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

void UMyAnimInstance::AnimNotify_HideClip(UAnimNotify* Notify)
{
	Owner->HideClip();
}

void UMyAnimInstance::AnimNotify_unhideClip(UAnimNotify* Notify)
{
	Owner->UnhideClip();
}

void UMyAnimInstance::AnimNotify_DropClip(UAnimNotify* Notify)
{
	Owner->DropClip();
}

void UMyAnimInstance::AnimNotify_GetNewClip(UAnimNotify* Notify)
{
	Owner->GetNewClip();
}

void UMyAnimInstance::AnimNotify_StartHookTrace(UAnimNotify* Notify)
{
	Owner->StartHookTrace();
}

void UMyAnimInstance::AnimNotify_EndHookTrace(UAnimNotify* Notify)
{
	Owner->EndHookTrace();
}

void UMyAnimInstance::AnimNotify_StartHookLTrace(UAnimNotify* Notify)
{
	Owner->StartHookLTrace();
}

void UMyAnimInstance::AnimNotify_EndHookLTrace(UAnimNotify* Notify)
{
	Owner->EndHookLTrace();
}

void UMyAnimInstance::AnimNotify_AmmoCheck(UAnimNotify* Notify)
{
	Owner->AmmoCheck();
}

void UMyAnimInstance::AnimNotify_Hadoken(UAnimNotify* Notify)
{
	Owner->Hadoken();
}

void UMyAnimInstance::GetEssentialValue()
{
	bADS = Owner->bADS;
	bCombatReady = Owner->bCombatReady;

	if (Owner->Pitch > 180)
		Pitch = Owner->Pitch - 360;
	else if (Owner->Pitch < -180)
		Pitch = Owner->Pitch + 360;
	else Pitch = Owner->Pitch;

	if (Owner->Yaw > 180)
		Yaw = Owner->Yaw - 360;
	else if(Owner->Yaw < -180)
		Yaw = Owner->Yaw + 360;
	else Yaw = Owner->Yaw;

	WeaponType = Owner->CurrentWeaponType;
	GripLOffset = Owner->GripLOffset;
	AimBaseLineOffset = Owner->AimBaseLineOffset;
	bLeftIK = Owner->bLeftIK;
	AimBaseLineRotator = Owner->AimBaseLineRotator;
	MagOffset = Owner->MagOffset;
	Speed = Owner->Ani_Speed;
	Direction = Owner->Ani_Direction;
	GripRType = Owner->CurrentGripRType;
//	bLeftIK = Owner->bCombatReady;
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

