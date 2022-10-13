// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EMyEnum.h"
#include "SMyStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSWeaponPanelInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AWeaponBase* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponType WeaponType;
};

USTRUCT(BlueprintType)
struct FSBuffInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ACharacterBase* Character;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EBuffType BuffType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BuffTime;
};
