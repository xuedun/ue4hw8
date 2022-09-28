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
	FVector LookAtPoint;
	FVector UpdateLookAtPoint();
	float Pitch;
	float Yaw;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		bool bIsPlaying;
	float Ani_Direction;
	float Ani_Speed;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void InputFirePressed();
	void InputFireReleased();

	void WeaponR();
	void WeaponL();
	void WeaponT();
	void ChangeToLastWeapon();
	void ChangeToNextWeapon();

	void Reload();

	void ChangeFireMode();
	void ADS();
	void StopADS();	
	
	void StartGame();

	void SpeedUp();
	void StopSpeedUp();
#pragma endregion

#pragma region Weapon
public:
	int8 MaxWeaponNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSWeaponPanelInfo> EquipWeapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartWithWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<AWeaponBase>> StartWeaponList;
	void StartWithWeapon();
	void PurchaseWeapon(UClass* Class);

	UPROPERTY(Replicated)
		bool bADS = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Replicated)
		bool bCombatReady = false;

	bool ReloadLock = false;
	bool ChangeWeaponLock = false;

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

	class AMyAmmo* Ammo;

//蒙太奇在蓝图中设置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UAnimMontage*> EquipWeaponMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UAnimMontage*> HolsterWeaponMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* HookMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* OneHandHipFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* OneHandADSFireMontage;

	void EquipWeapon(AWeaponBase* Weapon);
	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetCurrentWeapon();
#pragma endregion

#pragma region Fire
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bLineTraceFire = false;

	class AMyProjectile* Bullet;
	bool isFiring = false;
	UPROPERTY(EditAnywhere)
	bool FireLock = false;
	int BurstNum = 0;

	virtual void FireWeaponPrimary();
	void SpawnBulletLocal();

	//射线检测对应的检测和伤害计算
	void RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void DamagePlayer(UPhysicalMaterial* PhysicalMaterial,AActor* DamagedActor, FVector& HitFromDirection, FHitResult& HitInfo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HookDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HookBaseDamage;

#pragma endregion

#pragma region Health
	float Health;
	bool bDead = false;
	//受伤
	UFUNCTION()
		virtual void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION()
		virtual void OnHitAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	//死亡
	UFUNCTION(BlueprintCallable)
		void PVPDeath(AActor* DamageActor);
	UFUNCTION(BlueprintCallable)
		virtual void PVEDeath(AActor* DamageCauser);
public:
	bool Is_Alive();
#pragma endregion


#pragma region AnimationNotify
public:
	void GrabWeaponR();
	void OnHolster();
	void GrabWeaponLTemp();

	void HideClip();
	void UnhideClip();
	void DropClip();
	void GetNewClip();

	void StartHookTrace();
	void EndHookTrace();
	void StartHookLTrace();
	void EndHookLTrace();

	void AmmoCheck();

	FVector StartLocation;
	UFUNCTION(BlueprintCallable)
		void EquipWeaponAnimation();
	UFUNCTION(BlueprintCallable)
		void EndWeaponAnimation(class UAnimMontage* LastMontage);
	UFUNCTION(BlueprintCallable)
		void SingleAnimationAfterReload();
	bool AnimLock;
#pragma endregion


#pragma region Network
#pragma region Animation

#pragma endregion
	UFUNCTION(Client, Reliable)
		void ClientFire();

	UFUNCTION(Client, Reliable)
		void ClientRecoil();

	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmoUI(int32 ClipCurrentAmmo,int32 GunCurrentAmmo, bool HideAmmoUI);

	UFUNCTION(Client, Reliable)
		void ClientWeaponUI();

	UFUNCTION(Client, Reliable)
		void ClientUpdateHealthUI(float NewHealth);

	UFUNCTION(Client, Reliable)
		void ClientRespawn();

	UFUNCTION(Server, Reliable, WithValidation,BlueprintCallable)
		void DestoryAllWeapon();
	void DestoryAllWeapon_Implementation();
	bool DestoryAllWeapon_Validate();
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnBullet(UClass* Class, FTransform SpawnTransForm,FVector Velocity);
	void ServerSpawnBullet_Implementation(UClass* Class, FTransform SpawnTransForm, FVector Velocity);
	bool ServerSpawnBullet_Validate(UClass* Class, FTransform SpawnTransForm, FVector Velocity);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload();
	void ServerReload_Implementation();
	bool ServerReload_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeADSState(bool bNextADS);
	void ServerChangeADSState_Implementation(bool bNextADS);
	bool ServerChangeADSState_Validate(bool bNextADS);

	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerUpdataYP(float P, float Y);
	void ServerUpdataYP_Implementation(float P, float Y);
	bool ServerUpdataYP_Validate(float P, float Y);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastUpdataYP(float P, float Y);
	void MulticastUpdataYP_Implementation(float P, float Y);
	bool MulticastUpdataYP_Validate(float P, float Y);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastDieAnimation();
	void MulticastDieAnimation_Implementation();
	bool MulticastDieAnimation_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastEndSingleReloadAnimation();
	void MulticastEndSingleReloadAnimation_Implementation();
	bool MulticastEndSingleReloadAnimation_Validate();



	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHook(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void ServerHook_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	bool ServerHook_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(Server, Reliable,WithValidation)
		void ServerFireRifleWeapon(FVector CameraLocation,FRotator CameraRotation,bool IsMoving);
	void ServerFireRifleWeapon_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	bool ServerFireRifleWeapon_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastPlayAnimation(class UAnimMontage* Montage);
	void MulticastPlayAnimation_Implementation(class UAnimMontage* Montage);
	bool MulticastPlayAnimation_Validate(class UAnimMontage* Montage);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeWeaponAnimation(int8 index);
	void ServerChangeWeaponAnimation_Implementation(int8 index);
	bool ServerChangeWeaponAnimation_Validate(int8 index);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastChangeWeaponAnimation(int8 index);
	void MulticastChangeWeaponAnimation_Implementation(int8 index);
	bool MulticastChangeWeaponAnimation_Validate(int8 index);

#pragma endregion

#pragma region Hadoken
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyProjectile> HadokenBullet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* HadokenMontage;
	UPROPERTY(EditAnywhere)
		float HadokenVelocity = 300;
	UPROPERTY(EditAnywhere)
		float HadokenBaseDamege = 1;

	void HadokenAttack();
	void Hadoken();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHadoken();
	void ServerHadoken_Implementation();
	bool ServerHadoken_Validate();
#pragma endregion

#pragma region PoisonousFrog
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyDotProjectile> PoisonousFrog;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamagePerSec = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isPoisonous;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* PoisonousMontage;
	UPROPERTY(EditAnywhere)
		float PoisonousBasePeriod = 1.0f;
	UPROPERTY(EditAnywhere)
		float PoisonousBaseDamege = 1;

	void PoisonousAttack();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPoisonous();
	void ServerPoisonous_Implementation();
	bool ServerPoisonous_Validate();
#pragma endregion

#pragma region ThreeHookHit
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* ThreeHookHitMontage;
	UFUNCTION(BlueprintCallable)
	void ThreeHookHit();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerThreeHookHit();
	void ServerThreeHookHit_Implementation();
	bool ServerThreeHookHit_Validate();
#pragma endregion

#pragma region Buff
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastBuff(EBuffType Buff, bool bBuffState);
	void MulticastBuff_Implementation(EBuffType Buff, bool bBuffState);
	bool MulticastBuff_Validate(EBuffType Buff, bool bBuffState);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EBuffType,bool> BuffMap;
#pragma endregion

//后坐力
	float NewVerticalRecoilAmount;
	float OldVerticalRecoilAmount;
	float VerticalRecoilAmount;
	float RecoilXCoordPerShoot;
	float NewHorizontalRecoilAmount;
	float OldHorizontalRecoilAmount;
	float HorizontalRecoilAmount;

//子弹
	TMap<EBulletType, float> BulletNum;
	UFUNCTION(BlueprintCallable)
		void GetAmmoBullet();
//丢弃武器
	void ThrowWeapon();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerThrowWeapon();
	void ServerThrowWeapon_Implementation();
	bool ServerThrowWeapon_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastThrowWeapon();
	void MulticastThrowWeapon_Implementation();
	bool MulticastThrowWeapon_Validate();

// 狙击枪开镜
	UPROPERTY(EditAnywhere)
		float FieldOfAimingView;
	UPROPERTY()
		UUserWidget* WidgetScope;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> SniperScopeBPClass;
};
