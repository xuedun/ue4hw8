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
	FVector LookAtPoint;
	FVector UpdateLookAtPoint();
	UAnimInstance* AnimInstance;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion

#pragma region InputComponent
public:
	UPROPERTY(Replicated)
		float Pitch;
	UPROPERTY(Replicated)
		float Yaw;
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadWrite)
		bool bIsPlaying;
	bool bFrendHurt = true;
	float Ani_Direction;
	float Ani_Speed;

public:
	void InputFirePressed();
	void InputFireReleased();
	void ChangeToNextWeapon();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void ADS();
	void StopADS();	
	
	void WeaponR();
	void WeaponL();
	void WeaponT();


	void ChangeToLastWeapon();	

	void StartGame();
#pragma endregion

#pragma region Weapon
public:
	bool ChangeWeaponLock = false;
	UPROPERTY(Replicated)
	bool bADS = false;
	UPROPERTY(Replicated)
	bool bCombatReady = false;

	int8 MaxWeaponNum;
	int8 CurrentWeaponIndex;
	int8 TargetWeaponIndex;
	EWeaponType CurrentWeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector GripLOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator GripLRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector MagOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator MagRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector AimBaseLineOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator AimBaseLineRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bLeftIK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HookDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HookBaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSWeaponPanelInfo> EquipWeapons;
	class AMyAmmo* Ammo;

//换武器和开火蒙太奇在蓝图中设置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UAnimMontage*> EquipWeaponMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UAnimMontage*> HolsterWeaponMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* HookMontage;
	void EquipWeapon(AWeaponBase* Weapon);

	void StartWithWeapon();
	void PurchaseWeapon();

	void Reload();
public:
	AWeaponBase* GetCurrentWeapon();
#pragma endregion

#pragma region Fire
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		bool bLineTraceFire;
	class AMyProjectile* Bullet;
	//单发
	void Fire();
	virtual void FireWeaponPrimary();
	void StopFirePrimary();

	void RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void DamagePlayer(UPhysicalMaterial* PhysicalMaterial,AActor* DamagedActor, FVector& HitFromDirection, FHitResult& HitInfo);

	void SpawnBullet();
#pragma endregion

#pragma region Health
	float Health;
	bool bDead = false;
	UFUNCTION()
		virtual void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
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
	FVector StartLocation;
	UFUNCTION(BlueprintCallable)
		void EquipWeaponAnimation();
	UFUNCTION(BlueprintCallable)
		void EndWeaponAnimation();

#pragma endregion


#pragma region Network
#pragma region Animation

#pragma endregion
	UFUNCTION(Client, Reliable)
		void ClientFire();

	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmoUI(int32 ClipCurrentAmmo,int32 GunCurrentAmmo, bool HideAmmoUI);

	UFUNCTION(Client, Reliable)
		void ClientUpdateHealthUI(float NewHealth);

	UFUNCTION(Server, Reliable, WithValidation,BlueprintCallable)
		void DestoryAllWeapon();
	void DestoryAllWeapon_Implementation();
	bool DestoryAllWeapon_Validate();
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload();
	void ServerReload_Implementation();
	bool ServerReload_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHook(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void ServerHook_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	bool ServerHook_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(Server, Reliable,WithValidation)
		void ServerFireRifleWeapon(FVector CameraLocation,FRotator CameraRotation,bool IsMoving);
	void ServerFireRifleWeapon_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	bool ServerFireRifleWeapon_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeWeaponAnimation(int8 index);
	void ServerChangeWeaponAnimation_Implementation(int8 index);
	bool ServerChangeWeaponAnimation_Validate(int8 index);

	UFUNCTION(Server, Reliable, WithValidation)
		void ChangeADSState(bool bNextADS);
	void ChangeADSState_Implementation(bool bNextADS);
	bool ChangeADSState_Validate(bool bNextADS);

	UFUNCTION(Server, Unreliable, WithValidation)
		void ServerUpdataYP(float P, float Y);
	void ServerUpdataYP_Implementation(float P, float Y);
	bool ServerUpdataYP_Validate(float P, float Y);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastDieAnimation();
	void MulticastDieAnimation_Implementation();
	bool MulticastDieAnimation_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastPlayAnimation(class UAnimMontage* Montage);
	void MulticastPlayAnimation_Implementation(class UAnimMontage* Montage);
	bool MulticastPlayAnimation_Validate(class UAnimMontage* Montage);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastChangeWeaponAnimation(int8 index);
	void MulticastChangeWeaponAnimation_Implementation(int8 index);
	bool MulticastChangeWeaponAnimation_Validate(int8 index);


#pragma endregion


};
