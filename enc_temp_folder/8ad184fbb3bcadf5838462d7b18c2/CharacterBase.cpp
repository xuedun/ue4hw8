// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "WeaponBase.h"
#include "MyPlayerController.h"
#include "MyAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
#pragma region Component
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
#pragma endregion

#pragma region Weapon
	//EquipWeapons = { {TEXT("HosterBRSocket"),EWeaponType::TwoHandWeapon,nullptr},{TEXT("HosterBLSocket"),EWeaponType::TwoHandWeapon,nullptr} ,{TEXT("HosterThighRSocket"),EWeaponType::OneHandWeapon,nullptr} };
	FSWeaponPanelInfo temp = { TEXT("HosterBRSocket"),nullptr,EWeaponType::TwoHandWeapon};
	EquipWeapons.Add(temp);
	temp = { TEXT("HosterBLSocket"),nullptr,EWeaponType::TwoHandWeapon};
	EquipWeapons.Add(temp);
	temp = { TEXT("HosterThighRSocket"),nullptr,EWeaponType::OneHandWeapon};
	EquipWeapons.Add(temp);
	AimBaseLineOffset.Init(0, MaxWeaponNum);
	
#pragma endregion
}

#pragma region InputComponent
//////////////////////////////////////////////////////////////////////////
// Input

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharacterBase::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharacterBase::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACharacterBase::OnResetVR);


//	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::InputFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::InputFireReleased);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &ACharacterBase::ADS);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &ACharacterBase::StopADS);

	PlayerInputComponent->BindAction("WeaponR", IE_Pressed, this, &ACharacterBase::WeaponR);
	PlayerInputComponent->BindAction("WeaponL", IE_Pressed, this, &ACharacterBase::WeaponL);
	PlayerInputComponent->BindAction("WeaponT", IE_Pressed, this, &ACharacterBase::WeaponT);

	PlayerInputComponent->BindAction("ChangeToNextWeapon", IE_Pressed, this, &ACharacterBase::ChangeToNextWeapon);
	PlayerInputComponent->BindAction("ChangeToLastWeapon", IE_Pressed, this, &ACharacterBase::ChangeToLastWeapon);

}

void ACharacterBase::OnResetVR()
{
	// If TENFPS is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in TENFPS.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACharacterBase::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ACharacterBase::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ACharacterBase::TurnAtRate(float Rate)
{
//	Client_Yaw = Rate + Client_Yaw;
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(Rate);
}

void ACharacterBase::LookUpAtRate(float Rate)
{
//	Client_Pitch = Client_Pitch - Rate;
	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	AddControllerPitchInput(Rate);
}

void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
#pragma endregion

#pragma region Engine
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	Health = 100;
	OnTakePointDamage.AddDynamic(this,&ACharacterBase::OnHit);
	StartWithWeapon();
	PlayerController = Cast<AMyPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->CreatePlayerUI();
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator temp = GetBaseAimRotation() - K2_GetActorRotation();
	
//	if (!HasAuthority())
	if(IsLocallyControlled())
	{
		Pitch = temp.Pitch > 270 ? temp.Pitch - 360 : temp.Pitch;
		Yaw = temp.Yaw > 180 ? temp.Yaw - 360 : temp.Yaw;
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(Client_Pitch));
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(Client_Yaw));
		ServerUpdataYP(Pitch, Yaw);
	}
}
#pragma endregion

#pragma region Weapon
void ACharacterBase::EquipWeapon(AWeaponBase* Weapon)
{
	//		for (auto& WeaponSlot : EquipWeapons)
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		FSWeaponPanelInfo& WeaponSlot = EquipWeapons[i];
		if (!WeaponSlot.Weapon && WeaponSlot.WeaponType == Weapon->WeaponType)
		{
//			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Equip Weapon"));
			WeaponSlot.Weapon = Weapon;
			Weapon->K2_AttachToComponent(this->GetMesh(), WeaponSlot.SocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			if(!bCombatReady) CurrentWeaponIndex = i;
			Weapon->SetOwner(this);
			break;
		}
	}
}

void ACharacterBase::GrabWeaponR()
{
	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("hand_rSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	bCombatReady = true;
	CurrentWeaponType = EquipWeapons[CurrentWeaponIndex].WeaponType;
}

void ACharacterBase::OnHolster()
{
	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), EquipWeapons[CurrentWeaponIndex].SocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	bCombatReady = false;
}

void ACharacterBase::GrabWeaponLTemp()
{
//	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lSocket"), EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

}

void ACharacterBase::ADS()
{
	if (!bCombatReady) return;
	bADS = true;
	bUseControllerRotationYaw = true;
	ChangeADSState(true);
	return;
}

void ACharacterBase::StopADS()
{
	bADS = false;
	bUseControllerRotationYaw = false;
	ChangeADSState(false);
	return;
}

void ACharacterBase::EquipWeaponAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	CurrentWeaponIndex = TargetWeaponIndex;
	AnimInstance->Montage_Play(EquipWeaponMontages[CurrentWeaponIndex]);
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUFunction(this, TEXT("EndWeaponAnimation"));
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, EquipWeaponMontages[TargetWeaponIndex]);
}

void ACharacterBase::WeaponR()
{
	if (ChangeWeaponLock) return;
	ChangeWeaponLock = true;
	ServerChangeWeaponAnimation(0);
	return;
}

void ACharacterBase::WeaponL()
{
	if (ChangeWeaponLock) return;
	ChangeWeaponLock = true;
	ServerChangeWeaponAnimation(1);
	return;
}

void ACharacterBase::WeaponT()
{
	if (ChangeWeaponLock) return;
	ChangeWeaponLock = true;
	ServerChangeWeaponAnimation(2);
	return;
}

void ACharacterBase::ChangeToNextWeapon()
{
	if (ChangeWeaponLock) return;
	ChangeWeaponLock = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (EquipWeapons[CurrentWeaponIndex].Weapon && !bCombatReady)
	{
		ServerChangeWeaponAnimation(CurrentWeaponIndex);
		return;
	}
	int len = 3;
	int i = (CurrentWeaponIndex + 1) % len;
	while (i != CurrentWeaponIndex)
	{
		if (EquipWeapons[i].Weapon)
		{
			ServerChangeWeaponAnimation(i);
			break;
		}
		i = (i + 1) % len;
	}
}



void ACharacterBase::ChangeToLastWeapon()
{
	if (ChangeWeaponLock) return;
	ChangeWeaponLock = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (EquipWeapons[CurrentWeaponIndex].Weapon && !bCombatReady)
	{
		ServerChangeWeaponAnimation(CurrentWeaponIndex);
		return;
	}
	int len = 3;
	int i = CurrentWeaponIndex == 0 ? len - 1 : CurrentWeaponIndex - 1;
	while (i != CurrentWeaponIndex)
	{
		if (EquipWeapons[i].Weapon)
		{
			ServerChangeWeaponAnimation(i);
			break;
		}
		i = i == 0 ? len - 1 : i - 1;
	}
}


void ACharacterBase::EndWeaponAnimation()
{
	CurrentWeaponIndex = TargetWeaponIndex;
	ChangeWeaponLock = false;
	ClientUpdateAmmoUI(EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo, EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo);
}

void ACharacterBase::LocalFire()
{
	if (bCombatReady && EquipWeapons[CurrentWeaponIndex].Weapon)
	{
		EquipWeapons[CurrentWeaponIndex].Weapon->FireAnimation();
		//			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//			AnimInstance->Montage_Play(FireMontage);
		PlayerController->PlayerCameraShake(EquipWeapons[CurrentWeaponIndex].Weapon->CameraShakeClass);
		PlayerController->DoCrosshairRecoil();
	}
}

void ACharacterBase::StartWithWeapon()
{
	if (HasAuthority())
	{
		PurchaseWeapon();
	}
}

void ACharacterBase::PurchaseWeapon()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UClass* BlueprintVar = StaticLoadClass(AWeaponBase::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/BP_AR.BP_AR_C'"));
	AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(BlueprintVar,
		GetActorTransform(),
		SpawnInfo);
	//EquipWeapons[0].Weapon = Weapon;
	//Weapon->EquipWeapon();
	//EquipWeapon(Weapon);

}

#pragma endregion


#pragma region Fire
void ACharacterBase::Fire()
{
	if (EquipWeapons[CurrentWeaponIndex].Weapon && bCombatReady)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(FireMontage);
		EquipWeapons[CurrentWeaponIndex].Weapon->FireAnimation();
		return;
	}
}

void ACharacterBase::InputFirePressed()
{
	FireWeaponPrimary();
}

void ACharacterBase::InputFireReleased()
{

}

void ACharacterBase::RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	FVector EndLocation;
	FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);
	TArray<AActor*> IgnoreArray;
	FHitResult OutHit;
	IgnoreArray.Add(this);
	if (IsMoving)
	{

	}
	else
	{
		EndLocation = CameraLocation + CameraForwardVector * EquipWeapons[CurrentWeaponIndex].Weapon->BulletDistance;
	}
	bool HitSucess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::None, OutHit, true);
	if (HitSucess)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor Name : %s"), *OutHit.Actor->GetName()));
		ACharacterBase* HitCharacter = Cast<ACharacterBase>(OutHit.Actor);
		if (HitCharacter)
		{
			//命中玩家
			DamagePlayer(OutHit.PhysMaterial.Get(),OutHit.Actor.Get(), CameraForwardVector,OutHit);
		}
		else
		{

		}
	}
}

void ACharacterBase::DamagePlayer(UPhysicalMaterial* PhysicalMaterial, AActor* DamagedActor,FVector& HitFromDirection,FHitResult& HitInfo)
{
	float Damage;
	switch (PhysicalMaterial->SurfaceType) 
	{
	case EPhysicalSurface::SurfaceType1:
	{
		Damage = EquipWeapons[CurrentWeaponIndex].Weapon->BaseDamage * 4;
	}
	break;
	case EPhysicalSurface::SurfaceType2:
	{
		Damage = EquipWeapons[CurrentWeaponIndex].Weapon->BaseDamage * 1;
	}
	break;
	case EPhysicalSurface::SurfaceType3:
	{
		Damage = EquipWeapons[CurrentWeaponIndex].Weapon->BaseDamage * 0.8;
	}
	break;
	case EPhysicalSurface::SurfaceType4:
	{
		Damage = EquipWeapons[CurrentWeaponIndex].Weapon->BaseDamage * 0.6;
	}
	break;
	}
	UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, HitFromDirection,HitInfo,GetController(),this,UDamageType::StaticClass());
//	OnTakePointDamage.add
}

void ACharacterBase::OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	Health -= Damage;
	ClientUpdateHealthUI(Health);
	if (Health <= 0)
	{
		//死亡
		MulticastDieAnimation();
		for (int i = 0;i < MaxWeaponNum;i++)
		{
			if (EquipWeapons[i].Weapon)
			{
				EquipWeapons[i].Weapon->Destroy();
			}
		}
		PVPDeath(DamageCauser);
	}
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Health : %f"), Health));
}

void ACharacterBase::PVPDeath(AActor* DamageActor)
{
	AMyPlayerController* DiePlayerController = Cast<AMyPlayerController>(GetController());
	if (DiePlayerController)
	{
		DiePlayerController->PVPDeath(DamageActor);
	}
}

void ACharacterBase::FireWeaponPrimary()
{
	if (EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo <= 0) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Fire"));
	ServerFireRifleWeapon(GetFollowCamera()->GetComponentLocation(), GetFollowCamera()->GetComponentRotation(),false);
	ClientFire();
//	LocalFire();
}

void ACharacterBase::StopFirePrimary()
{

}

#pragma endregion

#pragma region Network


void ACharacterBase::ClientFire_Implementation()
{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(Pitch));
	if (true)
	{
		if (bCombatReady && EquipWeapons[CurrentWeaponIndex].Weapon)
		{
			EquipWeapons[CurrentWeaponIndex].Weapon->FireAnimation();
//			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//			AnimInstance->Montage_Play(FireMontage);
			if (PlayerController)
			{
				PlayerController->PlayerCameraShake(EquipWeapons[CurrentWeaponIndex].Weapon->CameraShakeClass);
				PlayerController->DoCrosshairRecoil();
			}
		}
	}
}

void ACharacterBase::ClientUpdateAmmoUI_Implementation(int32 ClipCurrentAmmo, int32 GunCurrentAmmo)
{
	if (PlayerController)
	{
		PlayerController->UpdateAmmoUI(ClipCurrentAmmo, GunCurrentAmmo);
	}
}

void ACharacterBase::ClientUpdateHealthUI_Implementation(float NewHealth)
{
	if (PlayerController)
	{
		PlayerController->UpdateHealthUI(NewHealth);
	}
}

void ACharacterBase::ServerFireRifleWeapon_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Fire"));
//	EquipWeapons[CurrentWeaponIndex].Weapon->MultiShootingEffect();
//	EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo -= 1;
//	MulticastPlayAnimation(FireMontage);
//	ClientUpdateAmmoUI(EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo, EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo);

//	RifleLineTrace(CameraLocation, CameraRotation, IsMoving);

}

bool ACharacterBase::ServerFireRifleWeapon_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	return true;
}


void ACharacterBase::ServerChangeWeaponAnimation_Implementation(int8 index)
{
	MulticastChangeWeaponAnimation(index);
}

bool ACharacterBase::ServerChangeWeaponAnimation_Validate(int8 index)
{
	return true;
}

void ACharacterBase::ChangeADSState_Implementation(bool bNextADS)
{
	bADS = bNextADS;
	bUseControllerRotationYaw = bNextADS;
}

bool ACharacterBase::ChangeADSState_Validate(bool bNextADS)
{
	return true;
}

void ACharacterBase::MulticastChangeWeaponAnimation_Implementation(int8 index)
{
	if (!EquipWeapons[index].Weapon) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (index == CurrentWeaponIndex && bCombatReady)
	{
		TargetWeaponIndex = index;
		AnimInstance->Montage_Play(HolsterWeaponMontages[index]);
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUFunction(this, TEXT("EndWeaponAnimation"));
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HolsterWeaponMontages[index]);
		return;
	}
	if (!bCombatReady)
	{
		TargetWeaponIndex = index;
		CurrentWeaponIndex = index;
		AnimInstance->Montage_Play(EquipWeaponMontages[index]);
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUFunction(this, TEXT("EndWeaponAnimation"));
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, EquipWeaponMontages[CurrentWeaponIndex]);
		return;
	}
	if (index != CurrentWeaponIndex && bCombatReady)
	{
		TargetWeaponIndex = index;
		AnimInstance->Montage_Play(HolsterWeaponMontages[CurrentWeaponIndex]);
	
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUFunction(this, TEXT("EquipWeaponAnimation"));
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HolsterWeaponMontages[CurrentWeaponIndex]);

		return;
	}
}

bool ACharacterBase::MulticastChangeWeaponAnimation_Validate(int8 index)
{
	return true;
}


void ACharacterBase::ServerUpdataYP_Implementation(float P, float Y)
{
	Pitch = P > 270 ? P - 360 : P;
	Yaw = Y > 180 ? Y - 360 : Y;
}

bool ACharacterBase::ServerUpdataYP_Validate(float P, float Y)
{
	return true;
}

void ACharacterBase::MulticastDieAnimation_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool ACharacterBase::MulticastDieAnimation_Validate()
{
	return true;
}

void ACharacterBase::MulticastPlayAnimation_Implementation(class UAnimMontage* Montage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(Montage);
}

bool ACharacterBase::MulticastPlayAnimation_Validate(class UAnimMontage* Montage)
{
	return true;
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, Yaw);
	DOREPLIFETIME(ACharacterBase, Pitch);
	DOREPLIFETIME(ACharacterBase, bADS);
	DOREPLIFETIME(ACharacterBase, bCombatReady);
}
#pragma endregion