// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "EMyEnum.h"
#include "MyCharacterBot.generated.h"

/**
 * 
 */
class UBehaviorTree;

UCLASS()
class TENFPS_API AMyCharacterBot : public ACharacterBase
{
	GENERATED_BODY()
public:
	AMyCharacterBot();

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		UBehaviorTree* BotBehaviorTree;
	UPROPERTY(EditAnywhere)
	AIType BotType;
	void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	virtual void PVEDeath(AActor* DamageCauser);
protected:
	virtual void BeginPlay() override;
};
