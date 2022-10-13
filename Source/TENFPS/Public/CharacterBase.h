// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMyStruct.h"
#include "Net/UnrealNetwork.h"
#include "CharacterBase.generated.h"

class AWeaponBase;

UCLASS()
class TENFPS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	ACharacterBase();

#pragma region Component
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
		class AMyPlayerController* PlayerController;
	UAnimInstance* AnimInstance;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion


#pragma region InputComponent
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void SpeedUp();
	void StopSpeedUp();
#pragma endregion

#pragma region Weapon
	int8 MaxWeaponNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSWeaponPanelInfo> EquipWeapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bStartWithWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<AWeaponBase>> StartWeaponList;
	void StartWithWeapon();
	void PurchaseWeapon(UClass* Class);
	void EquipWeapon(AWeaponBase* Weapon);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bCombatReady = false;
	UFUNCTION(BlueprintCallable)
		AWeaponBase* GetCurrentWeapon();

	int8 CurrentWeaponIndex;
	int8 TargetWeaponIndex;

	EWeaponType CurrentWeaponType;
	EGripRType CurrentGripRType;

	FVector GripLOffset;
	FRotator GripLRotator;
	FVector MagOffset;
	FRotator MagRotator;
	FVector AimBaseLineOffset;
	FRotator AimBaseLineRotator;
	bool bLeftIK;

	void ChangeFireMode();

#pragma endregion

#pragma region Base
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		bool bIsPlaying;

	FVector LookAtPoint;
	FVector UpdateLookAtPoint();

	float Ani_Speed;
	float Ani_Direction;

	float Pitch;
	float Yaw;
	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerUpdataYP(float P, float Y);
	void ServerUpdataYP_Implementation(float P, float Y);
	bool ServerUpdataYP_Validate(float P, float Y);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastUpdataYP(float P, float Y);
	void MulticastUpdataYP_Implementation(float P, float Y);
	bool MulticastUpdataYP_Validate(float P, float Y);

#pragma endregion

#pragma region ADS
	UPROPERTY(Replicated)
		bool bADS = false;
	void ADS();
	void StopADS();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeADSState(bool bNextADS);
	void ServerChangeADSState_Implementation(bool bNextADS);
	bool ServerChangeADSState_Validate(bool bNextADS);

	// 狙击枪开镜
	UPROPERTY(EditAnywhere)
		float FieldOfAimingView;
	UPROPERTY()
		UUserWidget* WidgetScope;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> SniperScopeBPClass;


#pragma endregion


#pragma region Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamagePerSec = 0;

	bool bDead = false;
	//受伤
	UFUNCTION()
		virtual void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION()
		virtual void OnHitAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//死亡
	void ServerDeath(class AController* InstigatedBy);

	bool Is_Alive();
	UFUNCTION(Client, Reliable)
		void ClientUpdateHealthUI(float NewHealth);
	//多播死亡动画
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastDieAnimation();
	void MulticastDieAnimation_Implementation();
	bool MulticastDieAnimation_Validate();

	UFUNCTION(Client, Reliable)
		void ClientRespawn();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void DestoryAllWeapon();
	void DestoryAllWeapon_Implementation();
	bool DestoryAllWeapon_Validate();
#pragma endregion 

#pragma region Reload
	bool ReloadLock = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* ReloadMontage;
	TMap<EBulletType, float> BulletNum;
	UFUNCTION(BlueprintCallable)
		void GetAmmoBullet();

	void Reload();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload();
	void ServerReload_Implementation();
	bool ServerReload_Validate();

	bool bHasAmmo = false;
	class AMyAmmo* Ammo;
	void HideClip();
	void UnhideClip();
	void DropClip();
	void GetNewClip();

	void AmmoCheck();

	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmoUI(int32 ClipCurrentAmmo, int32 GunCurrentAmmo, bool HideAmmoUI);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastEndSingleReloadAnimation();
	void MulticastEndSingleReloadAnimation_Implementation();
	bool MulticastEndSingleReloadAnimation_Validate();

	UFUNCTION(BlueprintCallable)
		void SingleAnimationAfterReload();
#pragma endregion

#pragma region Fire

	bool FireLock = false;
	void InputFirePressed();
	void InputFireReleased();

	virtual void FireWeaponPrimary();

	//客户端准星扩散以及镜头抖动以及后坐力
	UFUNCTION(Client, Reliable)
		void ClientFire();

	//后坐力
	float NewVerticalRecoilAmount;
	float OldVerticalRecoilAmount;
	float VerticalRecoilAmount;
	float RecoilXCoordPerShoot;
	float NewHorizontalRecoilAmount;
	float OldHorizontalRecoilAmount;
	float HorizontalRecoilAmount;
	UFUNCTION(Client, Reliable)
		void ClientRecoil();

	//服务端开火RPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* OneHandHipFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* OneHandADSFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* CharacterFireMontage;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFireRifleWeapon(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void ServerFireRifleWeapon_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	bool ServerFireRifleWeapon_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	//生成子弹
	class AMyProjectile* Bullet;
	UFUNCTION(Client, Reliable)
		void ClientSpawnBullet();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnBullet(UClass* Class, FTransform SpawnTransForm, FVector Velocity);
	void ServerSpawnBullet_Implementation(UClass* Class, FTransform SpawnTransForm, FVector Velocity);
	bool ServerSpawnBullet_Validate(UClass* Class, FTransform SpawnTransForm, FVector Velocity);

	//多播开火，计算连发
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastFireAnimation(class UAnimMontage* Montage);
	void MulticastFireAnimation_Implementation(class UAnimMontage* Montage);
	bool MulticastFireAnimation_Validate(class UAnimMontage* Montage);

	bool isFiring = false;
	int BurstNum = 0;
	UFUNCTION()
		void FireEndDelegate();

	//射线检测对应的检测和伤害计算
	UPROPERTY(EditAnywhere)
		bool bLineTraceFire = false;
	void RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void DamagePlayer(UPhysicalMaterial* PhysicalMaterial, AActor* DamagedActor, FVector& HitFromDirection, FHitResult& HitInfo);

#pragma endregion 

#pragma region SimpleAttack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyDotProjectile> PoisonousFrog;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* PoisonousMontage;
	UPROPERTY(EditAnywhere)
		float PoisonousBaseDamege = 1;
	UFUNCTION(BlueprintCallable)
		void PoisonousAttack();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPoisonous();
	void ServerPoisonous_Implementation();
	bool ServerPoisonous_Validate();

	//空手远程攻击
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyProjectile> HadokenBullet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* HadokenMontage;
	UPROPERTY(EditAnywhere)
		float HadokenVelocity = 300;
	UPROPERTY(EditAnywhere)
		float HadokenBaseDamege = 1;

	UFUNCTION(BlueprintCallable)
		void HadokenAttack();
	void Hadoken();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHadoken();
	void ServerHadoken_Implementation();
	bool ServerHadoken_Validate();

	//空手近战攻击
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HookBaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* HookMontage;
	UFUNCTION(BlueprintCallable)
		void Hook();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHook();
	void ServerHook_Implementation();
	bool ServerHook_Validate();
	//三连击
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* ThreeHookHitMontage;
	UFUNCTION(BlueprintCallable)
		void ThreeHookHit();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerThreeHookHit();
	void ServerThreeHookHit_Implementation();
	bool ServerThreeHookHit_Validate();
	//空手近战攻击伤害判定
	FVector StartLocation;
	void StartHookTrace();
	void EndHookTrace();
	void StartHookLTrace();
	void EndHookLTrace();
#pragma endregion

#pragma region Buff
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastBuff(EBuffType Buff, bool bBuffState);
	void MulticastBuff_Implementation(EBuffType Buff, bool bBuffState);
	bool MulticastBuff_Validate(EBuffType Buff, bool bBuffState);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EBuffType,bool> BuffMap;
#pragma endregion

//丢弃武器
#pragma region Throw
	void ThrowWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerThrowWeapon();
	void ServerThrowWeapon_Implementation();
	bool ServerThrowWeapon_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastThrowWeapon();
	void MulticastThrowWeapon_Implementation();
	bool MulticastThrowWeapon_Validate();
#pragma endregion


// 更换武器
#pragma region ChangeWeapon
public:
	bool ChangeWeaponLock = false;
	UPROPERTY(EditAnywhere)
		TArray<class UAnimMontage*> EquipWeaponMontages;
	UPROPERTY(EditAnywhere)
		TArray<class UAnimMontage*> HolsterWeaponMontages;

	void WeaponR();
	void WeaponL();
	void WeaponT();
	void ChangeToLastWeapon();
	void ChangeToNextWeapon();
//RPC
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeWeaponAnimation(int8 index);
	void ServerChangeWeaponAnimation_Implementation(int8 index);
	bool ServerChangeWeaponAnimation_Validate(int8 index);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastChangeWeaponAnimation(int8 index);
	void MulticastChangeWeaponAnimation_Implementation(int8 index);
	bool MulticastChangeWeaponAnimation_Validate(int8 index);
//委托
	UFUNCTION(BlueprintCallable)
		void EquipWeaponAnimation();
//蒙太奇骨骼通知
	void GrabWeaponR();
	void OnHolster();
	void GrabWeaponLTemp();
//武器界面UI
	UFUNCTION(Client, Reliable)
		void ClientWeaponUI();
#pragma endregion 

#pragma region Animation

	//RPC
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastPlayAnimation(class UAnimMontage* Montage);
	void MulticastPlayAnimation_Implementation(class UAnimMontage* Montage);
	bool MulticastPlayAnimation_Validate(class UAnimMontage* Montage);

	//委托
	UFUNCTION(BlueprintCallable)
		void UnlockAfterAnimation();
#pragma endregion
};
