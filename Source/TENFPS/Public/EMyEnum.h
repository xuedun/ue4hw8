// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EMyEnum.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API UEMyEnum : public UObject
{
	GENERATED_BODY()
	
};

UENUM(BlueprintType)
enum class EBulletType : uint8
{
	AR,
	ShotGun,
	Pistol,
	Snaper
};

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	Bullet,
	Attack,
	Health,
	Defence,
	RPM
};

UENUM(BlueprintType)
enum class EGripRType : uint8
{
	Standard,
	ShotGun
};

UENUM(BlueprintType)
enum class AIType : uint8
{
	AR,
	Hook
};

UENUM(BlueprintType)
enum class GaState : uint8
{
	Start,
	MainMenu,
	ServerList,
	LoadingScreen,
	Playing
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	TwoHandWeapon,
	OneHandWeapon,
	TwoHandCompact,
	ThrowWeapon
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Single,
	Semi_Auto,
	Burst,
	Full_Auto
};
