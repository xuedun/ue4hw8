// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "EMyEnum.h"
#include "MyAmmo.h"
#include "MyProjectile.h"
#include "WeaponBase.generated.h"

UCLASS()
class TENFPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
//Component 包括枪骨骼网格体以及碰撞体

#pragma region Component
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereCollision;
	AWeaponBase();
#pragma endregion
#pragma region BaseInformation
public:
	//每把枪需要单独设置的信息：武器类型（单手武器，双手武器，近战武器。。。
	//	可支持的开火模式（半自动、全自动、三连发、单发）开火速度（每秒多少发） 开火蒙太奇 开火镜头shake、开火粒子效果、开火音效
	//	子弹类、子弹射速、有效射程、基础伤害
	//	弹夹类、总子弹数、弹夹内子弹数、最大弹夹子弹数、子弹散布数量、
	UPROPERTY(EditAnywhere)
		int WeaponIndex;
	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere)
		EGripRType GripRType;

	UPROPERTY(EditAnywhere)
		TArray<EFireMode> AvailableFireMode;
		int8 CurrentFireModeIndex;
	UPROPERTY(EditAnywhere)
		EFireMode CurrentFireMode;
	UPROPERTY(EditAnywhere)
		float RPM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* WeaponFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* WeaponSingleMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* CharacterFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* WeaponReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* CharacterReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UCameraShakeBase> CameraShakeClass;
	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)
		USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AMyProjectile> Bullet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BulletVelocity;
	UPROPERTY(EditAnywhere)
		float BulletDistance;
	UPROPERTY(EditAnywhere)
		float BaseDamage;
	UPROPERTY(EditAnywhere)
		int8 PelletPerShot = 1;
	UPROPERTY(EditAnywhere)
		float MaxSpreadAngle = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AMyAmmo> Ammo;
	UPROPERTY(EditAnywhere, Replicated)
		int32 ClipCurrentAmmo;
	UPROPERTY(EditAnywhere)
		int32 MaxClipAmmo;
	UPROPERTY(EditAnywhere)
		EBulletType BulletType;
	UPROPERTY(EditAnywhere)
		bool IsSingleReload;

	UPROPERTY(EditAnywhere)
		UCurveFloat* VerticalRecoilCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* HorizontalRecoilCurve;
#pragma endregion

public:	
//重叠触发武器拾取
	float AimBaselineOffset;
	UFUNCTION()
		void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OterComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	void EquipWeapon();

#pragma region Animation
	void FireAnimation(float MotagePlayRate);
	UFUNCTION()
	void EndAnimation(class UAnimMontage* Montage);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastAnimation(class UAnimMontage* Montage);
	void MulticastAnimation_Implementation(class UAnimMontage* Montage);
	bool MulticastAnimation_Validate(class UAnimMontage* Montage);
	void ShootingEffect();
#pragma endregion
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
