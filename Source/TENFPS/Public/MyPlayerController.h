// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EMyEnum.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class ACharacterBase* Chara;
	UFUNCTION(BlueprintCallable)
	void UpdateDotDamage();
	void PlayerCameraShake(TSubclassOf<UCameraShakeBase> CameraShake);
	UFUNCTION(BlueprintImplementableEvent,Category="PlayerUI")
		void CreatePlayerUI();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
		void DoCrosshairRecoil();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
		void UpdateAmmoUI(int32 ClipCurrentAmmo, int32 GunCurrentAmmo, bool bHideAmmoUI);
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerUI")
		void UpdateHealthUI(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
		void PVPDeath(AActor* DamageActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
		void PVEDeath();
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
		void RespawnDownTime();

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
		void UpdateBuffUI();
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
		void GetBuff(EBuffType Buff, float BuffTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
		void EndBuff(EBuffType Buff);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateWeaponUI(int32 WeaponIndex, FName FireMode);
};
