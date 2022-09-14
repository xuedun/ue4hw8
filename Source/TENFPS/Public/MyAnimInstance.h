// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EMyEnum.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AnimNotify")
		void AnimNotify_GrabWeaponR(UAnimNotify* Notify);
	UFUNCTION(BlueprintCallable, Category = "AnimNotify")
		void AnimNotify_OnHolster(UAnimNotify* Notify);
	UFUNCTION(BlueprintCallable, Category = "AnimNotify")
		void AnimNotify_GrabWeaponLTemp(UAnimNotify* Notify);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ACharacterBase* Owner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bADS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCombatReady;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Yaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AimBaselineOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponType WeaponType;

	UFUNCTION(BlueprintCallable)
		void GetEssentialValue();
	////UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	//UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable)
	//	void GetValue();
	//void GetValue_Implementation();
	//bool GetValue_Validate();
};
