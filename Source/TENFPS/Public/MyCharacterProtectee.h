// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "MyCharacterProtectee.generated.h"

/**
 * 
 */
class UBehaviorTree;

UCLASS()
class TENFPS_API AMyCharacterProtectee : public ACharacterBase
{
	GENERATED_BODY()
public:
	AMyCharacterProtectee();

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		UBehaviorTree* BotBehaviorTree;

	void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;



};
