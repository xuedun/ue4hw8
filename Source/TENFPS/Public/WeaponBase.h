// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "EMyEnum.h"
#include "WeaponBase.generated.h"

UCLASS()
class TENFPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* WeaponFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UCameraShakeBase> CameraShakeClass;
	UFUNCTION()
		void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OterComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	UFUNCTION()
		void EquipWeapon();
	UFUNCTION(BlueprintCallable)
		void FireAnimation();
	float AimBaselineOffset;
public:	
	AWeaponBase();
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MultiShootingEffect();
	void MultiShootingEffect_Implementation();
	bool MultiShootingEffect_Validate();

	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)
		USoundBase* FireSound;
	UPROPERTY(EditAnywhere, Replicated)
		int32 GunCurrentAmmo;
	UPROPERTY(EditAnywhere,Replicated)
		int32 ClipCurrentAmmo;
	UPROPERTY(EditAnywhere)
		int32 MaxClipAmmo;
	UPROPERTY(EditAnywhere)
		float BulletDistance;
	UPROPERTY(EditAnywhere)
		float BaseDamage;
};
