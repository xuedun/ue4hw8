// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
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
};
