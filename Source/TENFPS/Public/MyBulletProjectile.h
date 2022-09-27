// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectile.h"
#include "MyBulletProjectile.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyBulletProjectile : public AMyProjectile
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
