// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectile.h"
#include "MyDotProjectile.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyDotProjectile : public AMyProjectile
{
	GENERATED_BODY()
	AMyDotProjectile();
	UPROPERTY(EditAnywhere)
		float BaseDamage;
	void OnHit(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void StopOnHit(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	bool bDelay = true;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
