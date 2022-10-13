// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "MyCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyCharacterPlayer : public ACharacterBase
{
	GENERATED_BODY()
	virtual void FireWeaponPrimary();
	UFUNCTION(Client, Reliable)
		void ClientCreateUI();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
