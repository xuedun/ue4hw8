// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMyStruct.h"
#include "Net/UnrealNetwork.h"
#include "CharacterBase.generated.h"


UCLASS()
class TENFPS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
#pragma region Component
		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
private:
	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	class AMyPlayerController* PlayerController;
//	UAnimInstance* AnimInstance;
#pragma endregion
public:
	ACharacterBase();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


#pragma region Weapon

	int8 CurrentWeaponIndex;
	int8 TargetWeaponIndex;
	int8 MaxWeaponNum = 3;

	bool ChangeWeaponLock=false;
	FVector LookAtPoint;
	UPROPERTY(Replicated)
	bool bADS = false;
	UPROPERTY(Replicated)
	bool bCombatReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> AimBaseLineOffset;

	EWeaponType CurrentWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSWeaponPanelInfo> EquipWeapons;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UAnimMontage*> EquipWeaponMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class UAnimMontage*> HolsterWeaponMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* FireMontage;

	void EquipWeapon(AWeaponBase* Weapon);


#pragma region Fire
public:
	//单发
	void Fire();
	void InputFirePressed();
	void InputFireReleased();

	
	void FireWeaponPrimary();
	void StopFirePrimary();
	void RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving);
	void DamagePlayer(UPhysicalMaterial* PhysicalMaterial,AActor* DamagedActor, FVector& HitFromDirection, FHitResult& HitInfo);
	UFUNCTION()
		void OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	float Health;

	void PVPDeath(AActor* DamageActor);
#pragma endregion


	void ADS();
	void StopADS();

	void ClientChangeWeaponAnimation(int8 index);

	void WeaponR();
	void WeaponL();
	void WeaponT();

	void ChangeToNextWeapon();
	void ChangeToLastWeapon();


	void StartWithWeapon();
	void PurchaseWeapon();

	UFUNCTION(BlueprintCallable)
		void EquipWeaponAnimation();
	UFUNCTION(BlueprintCallable)
		void EndWeaponAnimation();

#pragma endregion

#pragma region AnimationNotify
	void GrabWeaponR();
	void OnHolster();
	void GrabWeaponLTemp();
#pragma endregion
public:
	UPROPERTY(Replicated)
		float Pitch;
	UPROPERTY(Replicated)
		float Yaw;
	float Client_Pitch, Client_Yaw;




#pragma region Network
	UFUNCTION(Client, Reliable)
		void ClientFire();

	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmoUI(int32 ClipCurrentAmmo,int32 GunCurrentAmmo);

	UFUNCTION(Client, Reliable)
		void ClientUpdateHealthUI(float NewHealth);

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
