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
#include "MyGameInstance.h"
#include "MyGameModeBase.h"
#include "MyAmmo.h"
#include "MyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
#pragma region Component
	bIsPlaying = false;
	bDead = false;
	bCombatReady = false;
	JumpMaxCount = 2;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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
	PlayerController = Cast<AMyPlayerController>(GetController());
	
#pragma endregion

#pragma region Weapon
	//EquipWeapons = { {TEXT("HosterBRSocket"),EWeaponType::TwoHandWeapon,nullptr},{TEXT("HosterBLSocket"),EWeaponType::TwoHandWeapon,nullptr} ,{TEXT("HosterThighRSocket"),EWeaponType::OneHandWeapon,nullptr} };
	MaxWeaponNum = 3;

	//初始化枪械栏位
	FSWeaponPanelInfo temp = { TEXT("HosterBRSocket"),nullptr,EWeaponType::TwoHandWeapon};
	EquipWeapons.Add(temp);
	temp = { TEXT("HosterBLSocket"),nullptr,EWeaponType::TwoHandWeapon};
	EquipWeapons.Add(temp);
	temp = { TEXT("HosterThighRSocket"),nullptr,EWeaponType::OneHandWeapon};
	EquipWeapons.Add(temp);

	
	
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

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharacterBase::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharacterBase::TouchStopped);

//	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::Fire);
	PlayerInputComponent->BindAction("StartGame", IE_Pressed, this, &ACharacterBase::StartGame);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACharacterBase::Reload);

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

void ACharacterBase::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ACharacterBase::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
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

void ACharacterBase::InputFirePressed()
{
	switch (GetCurrentWeapon()->CurrentFireMode)
	{
	case EFireMode::Semi_Auto:
		FireWeaponPrimary();
		break;
	default:
		FireWeaponPrimary();
	}
}

void ACharacterBase::InputFireReleased()
{

}

void ACharacterBase::StartGame()
{
	if (bIsPlaying) return;
	if (HasAuthority() && IsLocallyControlled())
	{
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("GameStart")));
		AMyGameModeBase* Gamemode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		Gamemode->GameModeStartGame();
	}
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
	if(!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
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
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
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

#pragma endregion

#pragma region Engine
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	Health = 100;
//	OnTakePointDamage.AddDynamic(this,&ACharacterBase::OnHit);
	StartWithWeapon();

	PlayerController = Cast<AMyPlayerController>(GetController());
	
	if (PlayerController)
	{
		PlayerController->CreatePlayerUI();
	}
	else {
		PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			PlayerController->CreatePlayerUI();
		}
	}
	AnimInstance = GetMesh()->GetAnimInstance();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator temp = GetBaseAimRotation() - K2_GetActorRotation();
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%i"),bCombatReady));
//	if (!HasAuthority())
	if(IsLocallyControlled())
	{
		Pitch = temp.Pitch > 270 ? temp.Pitch - 360 : temp.Pitch;
		Yaw = temp.Yaw > 180 ? temp.Yaw - 360 : temp.Yaw;
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(Client_Pitch));
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(Client_Yaw));
		ServerUpdataYP(Pitch, Yaw);
	}
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	bLeftIK = bCombatReady && (AnimInstance->GetCurveValue("LeftHandIKOff") == 0);

	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	Ani_Speed = GetVelocity().Size();
	Ani_Direction = AnimInstance->CalculateDirection(GetVelocity(), GetActorRotation());
//	UpdateLookAtPoint();
}


FVector ACharacterBase::UpdateLookAtPoint()
{
	FVector CameraLocation = GetFollowCamera()->GetComponentLocation();
	TArray<AActor*> IgnoreArray;
	FHitResult OutHit;
	IgnoreArray.Add(this);
	FVector EndLocation = CameraLocation + UKismetMathLibrary::GetForwardVector(GetControlRotation()) * 10000;
	bool HitSucess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::None, OutHit, true);
	LookAtPoint = HitSucess ? OutHit.Location : OutHit.TraceEnd;
	return LookAtPoint;
}

#pragma endregion

#pragma region Weapon
void ACharacterBase::StartWithWeapon()
{
	
	if (HasAuthority())
	{
		PurchaseWeapon();
		return;
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
}

void ACharacterBase::Reload()
{
	if ((!bCombatReady) || EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo <= 0) return;
	ServerReload();
}

AWeaponBase* ACharacterBase::GetCurrentWeapon()
{
	if (bCombatReady) return EquipWeapons[CurrentWeaponIndex].Weapon;
	else return nullptr;
}

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

#pragma endregion


#pragma region Fire
//待删除
void ACharacterBase::Fire()
{
	if (EquipWeapons[CurrentWeaponIndex].Weapon && bCombatReady)
	{
		if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(FireMontage);
		EquipWeapons[CurrentWeaponIndex].Weapon->FireAnimation();
		return;
	}
}

void ACharacterBase::FireWeaponPrimary()
{
	if (ChangeWeaponLock) return;
	ChangeWeaponLock = true;
	if (!bCombatReady) {
	//未装备武器时默认近战攻击
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ServerHook")));
		ServerHook(GetFollowCamera()->GetComponentLocation(), GetFollowCamera()->GetComponentRotation(), false);
		return;
	}
	else 
	{
		if (EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo <= 0)
		{
			Reload();
		}
	//服务器端多播开火特效，开火动画，计算子弹减少，更新弹夹UI，进行射线检测
	//客户端准星扩散以及镜头抖动
		if (EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo > 0)
		{
			ServerFireRifleWeapon(GetFollowCamera()->GetComponentLocation(), GetFollowCamera()->GetComponentRotation(), false);

//			ClientFire();
		}
	}
	ChangeWeaponLock = false;
}

void ACharacterBase::StopFirePrimary()
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
		if (bCombatReady) EndLocation = CameraLocation + CameraForwardVector * GetCurrentWeapon()->BulletDistance;
		else EndLocation = CameraLocation + CameraForwardVector * HookDistance;
	}
	bool HitSucess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::Persistent, OutHit, true);
	if (HitSucess)
	{
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit Actor Name : %s"), *OutHit.Actor->GetName()));
		ACharacterBase* HitCharacter = Cast<ACharacterBase>(OutHit.Actor);
		if (HitCharacter)
		{
			//命中玩家
			if(bFrendHurt) DamagePlayer(OutHit.PhysMaterial.Get(),OutHit.Actor.Get(), CameraForwardVector,OutHit);
		}
		else
		{

		}
	}
}

void ACharacterBase::DamagePlayer(UPhysicalMaterial* PhysicalMaterial, AActor* DamagedActor,FVector& HitFromDirection,FHitResult& HitInfo)
{
	float Damage;
	float BaseDamage;
	if (bCombatReady) BaseDamage = EquipWeapons[CurrentWeaponIndex].Weapon->BaseDamage;
	else BaseDamage = HookBaseDamage;
	switch (PhysicalMaterial->SurfaceType) 
	{
	case EPhysicalSurface::SurfaceType1:
	{
		Damage = BaseDamage * 4;
	}
	break;
	case EPhysicalSurface::SurfaceType2:
	{
		Damage = BaseDamage * 1;
	}
	break;
	case EPhysicalSurface::SurfaceType3:
	{
		Damage = BaseDamage * 0.8;
	}
	break;
	case EPhysicalSurface::SurfaceType4:
	{
		Damage = BaseDamage * 0.6;
	}
	break;
	}
	UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, HitFromDirection,HitInfo,GetController(),this,UDamageType::StaticClass());
}




void ACharacterBase::SpawnBullet()
{
	FVector Start = GetCurrentWeapon()->WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
	FVector Velocity = (UpdateLookAtPoint() - Start);
//	TArray<AActor*> IgnoreArray;
//	FHitResult OutHit;
//	IgnoreArray.Add(this);
//	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, Start + Velocity.GetSafeNormal()*100, ETraceTypeQuery::TraceTypeQuery1, false,
//		IgnoreArray, EDrawDebugTrace::Persistent, OutHit, true))
//	{
//		DamagePlayer(OutHit.PhysMaterial.Get(), OutHit.Actor.Get(), Velocity, OutHit);
//		return;
//	}
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = GetCurrentWeapon();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	//子弹速度向量方向从枪口指向屏幕中心
	
	Velocity = (Yaw >= 90 || Yaw <= -90) ? GetCurrentWeapon()->WeaponMesh->GetSocketRotation(TEXT("Muzzle")).Vector() : Velocity;

	Bullet = GetWorld()->SpawnActor<AMyProjectile>(GetCurrentWeapon()->Bullet,
		GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("Muzzle"), ERelativeTransformSpace::RTS_World),
		SpawnInfo);
	while (!Bullet)
	{
		Bullet = GetWorld()->SpawnActor<AMyProjectile>(GetCurrentWeapon()->Bullet,
			GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("Muzzle"), ERelativeTransformSpace::RTS_World),
			SpawnInfo);
	}
	Bullet->ProjectileMovementComponent->Velocity = Velocity.GetSafeNormal() * GetCurrentWeapon()->BulletVelocity;
	Bullet = nullptr;
}

#pragma endregion

#pragma region Health
void ACharacterBase::OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
//	if (!bIsPlaying) Damage = 0;
//	if (InstigatedBy);
	if (bDead) return;
	Health -= Damage;
	DamageCauser->Destroy();
	ClientUpdateHealthUI(Health);
	if (Health <= 0 && !bDead)
	{
		//死亡
		bDead = true;
		MulticastDieAnimation();
		if (bIsPlaying)
		{
			PVEDeath(InstigatedBy->GetPawn());
			return;
		}
		else
		{
			PVPDeath(InstigatedBy->GetPawn());
			return;
		}	
	}
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Health : %f"), Health));
}

void ACharacterBase::PVPDeath(AActor* DamageActor)
{
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon)
		{
			EquipWeapons[i].Weapon->Destroy();
		}
	}
	AMyPlayerController* DiePlayerController = Cast<AMyPlayerController>(GetController());
//	UKismetSystemLibrary::PrintString(GetWorld(), UKismetSystemLibrary::GetObjectName(DiePlayerController));
	if (DiePlayerController)
	{
		DiePlayerController->PVPDeath(DamageActor);
	}
}

void ACharacterBase::PVEDeath(AActor* DamageCauser)
{
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon)
		{
			EquipWeapons[i].Weapon->Destroy();
		}
	}
	AMyPlayerController* DiePlayerController = Cast<AMyPlayerController>(GetController());
	//	UKismetSystemLibrary::PrintString(GetWorld(), UKismetSystemLibrary::GetObjectName(DiePlayerController));
	if (DiePlayerController)
	{
		DiePlayerController->PVEDeath();
	}
}


bool ACharacterBase::Is_Alive()
{
	return !bDead;
}

#pragma endregion
#pragma region AnimationNotify
void ACharacterBase::GrabWeaponR()
{
	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("hand_rSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	CurrentWeaponType = EquipWeapons[CurrentWeaponIndex].WeaponType;
	bCombatReady = true;
	FTransform GripTransform = EquipWeapons[CurrentWeaponIndex].Weapon->WeaponMesh->GetSocketTransform(TEXT("GripL"), ERelativeTransformSpace::RTS_World);
	GetMesh()->TransformToBoneSpace(TEXT("hand_r"), GripTransform.GetLocation(), FRotator::ZeroRotator, GripLOffset, GripLRotator);
//	AimBaseLineOffset = UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange(EquipWeapons[CurrentWeaponIndex].Weapon->WeaponMesh, (TEXT("Root"), ERelativeTransformSpace::RTS_World, TEXT("Sight"), ERelativeTransformSpace::RTS_World, false, 0, 0, 0, 0) - 13.5;
	FTransform GripRTransform = EquipWeapons[CurrentWeaponIndex].Weapon->WeaponMesh->GetSocketTransform(TEXT("Sight"), ERelativeTransformSpace::RTS_World);
	GetMesh()->TransformToBoneSpace(TEXT("hand_r"), GripRTransform.GetLocation(), FRotator::ZeroRotator, AimBaseLineOffset, AimBaseLineRotator);
	FTransform MagTransform = EquipWeapons[CurrentWeaponIndex].Weapon->WeaponMesh->GetSocketTransform(TEXT("Mag"), ERelativeTransformSpace::RTS_World);
	GetMesh()->TransformToBoneSpace(TEXT("hand_r"), MagTransform.GetLocation(), FRotator::ZeroRotator, MagOffset, MagRotator);

}

void ACharacterBase::OnHolster()
{
	bCombatReady = false;
	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), EquipWeapons[CurrentWeaponIndex].SocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
}

void ACharacterBase::GrabWeaponLTemp()
{
//	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lSocket"), EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
}

void ACharacterBase::HideClip()
{
	EquipWeapons[CurrentWeaponIndex].Weapon->WeaponMesh->HideBoneByName(TEXT("Magazine"), EPhysBodyOp::PBO_None);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Ammo = GetWorld()->SpawnActor<AMyAmmo>(EquipWeapons[CurrentWeaponIndex].Weapon->Ammo,
		GetMesh()->GetSocketTransform(TEXT("hand_lClipSocket"), ERelativeTransformSpace::RTS_World),
		SpawnInfo);
	Ammo->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lClipSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
}

void ACharacterBase::UnhideClip()
{
	Ammo->K2_DestroyActor();
	EquipWeapons[CurrentWeaponIndex].Weapon->WeaponMesh->UnHideBoneByName(TEXT("Magazine"));
}

void ACharacterBase::DropClip()
{
	Ammo->Mesh->K2_DetachFromComponent(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	Ammo->Mesh->SetEnableGravity(true);
	Ammo->Mesh->SetSimulatePhysics(true);
}

void ACharacterBase::GetNewClip()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Ammo = GetWorld()->SpawnActor<AMyAmmo>(EquipWeapons[CurrentWeaponIndex].Weapon->Ammo,
		GetMesh()->GetSocketTransform(TEXT("hand_lClipSocket"), ERelativeTransformSpace::RTS_World),
		SpawnInfo);
	Ammo->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lClipSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
}

void ACharacterBase::StartHookTrace()
{
	StartLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
}

void ACharacterBase::EndHookTrace()
{
	TArray<AActor*> IgnoreArray;
	TArray<FHitResult> OutHit;
	IgnoreArray.Add(this);
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon) IgnoreArray.Add(EquipWeapons[i].Weapon);
	}
	FVector EndLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLocation, EndLocation,20, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::Persistent, OutHit, true);
	ChangeWeaponLock = false;
}

void ACharacterBase::EquipWeaponAnimation()
{
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	CurrentWeaponIndex = TargetWeaponIndex;
	AnimInstance->Montage_Play(EquipWeaponMontages[CurrentWeaponIndex]);
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUFunction(this, TEXT("EndWeaponAnimation"));
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, EquipWeaponMontages[TargetWeaponIndex]);
}

void ACharacterBase::EndWeaponAnimation()
{
	CurrentWeaponIndex = TargetWeaponIndex;
	ChangeWeaponLock = false;
	ClientUpdateAmmoUI(EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo, EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo,!bCombatReady);
}

#pragma endregion

#pragma region Network
#pragma region Animation

#pragma endregion
void ACharacterBase::ClientFire_Implementation()
{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(Pitch));
	//客户端准星扩散以及镜头抖动
	if (true)
	{
		if (bCombatReady && EquipWeapons[CurrentWeaponIndex].Weapon)
		{
			EquipWeapons[CurrentWeaponIndex].Weapon->FireAnimation();
			if (PlayerController)
			{
				PlayerController->PlayerCameraShake(EquipWeapons[CurrentWeaponIndex].Weapon->CameraShakeClass);
				PlayerController->DoCrosshairRecoil();
			}
		}
	}
}

void ACharacterBase::ClientUpdateAmmoUI_Implementation(int32 ClipCurrentAmmo, int32 GunCurrentAmmo,bool HideAmmoUI)
{
	if (PlayerController)
	{
		PlayerController->UpdateAmmoUI(ClipCurrentAmmo, GunCurrentAmmo,HideAmmoUI);
	}
}

void ACharacterBase::ClientUpdateHealthUI_Implementation(float NewHealth)
{
	if (PlayerController)
	{
		PlayerController->UpdateHealthUI(NewHealth);
	}
}

void ACharacterBase::DestoryAllWeapon_Implementation()
{
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon)
		{
			EquipWeapons[i].Weapon->Destroy();
		}
	}
}

bool ACharacterBase::DestoryAllWeapon_Validate()
{
	return true;
}

void ACharacterBase::ServerReload_Implementation()
{
	MulticastPlayAnimation(ReloadMontage);
	if (EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo + EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo > EquipWeapons[CurrentWeaponIndex].Weapon->MaxClipAmmo)
	{
		EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo = EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo + EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo - EquipWeapons[CurrentWeaponIndex].Weapon->MaxClipAmmo;
		EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo = EquipWeapons[CurrentWeaponIndex].Weapon->MaxClipAmmo;
	}
	else
	{
		EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo += EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo;
		EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo = 0;
	}
	ClientUpdateAmmoUI(EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo, EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo, !bCombatReady);

}

bool ACharacterBase::ServerReload_Validate()
{
	return true;
}

void ACharacterBase::ServerHook_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	MulticastPlayAnimation(HookMontage);
//	RifleLineTrace(CameraLocation, CameraRotation, IsMoving);
}

bool ACharacterBase::ServerHook_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	return true;
}

void ACharacterBase::ServerFireRifleWeapon_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Fire"));
	//服务器端多播开火特效，开火动画，计算子弹减少，更新弹夹UI，进行射线检测
	EquipWeapons[CurrentWeaponIndex].Weapon->MultiShootingEffect();
	if(bIsPlaying) EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo -= 1;
	MulticastPlayAnimation(FireMontage);
	ClientUpdateAmmoUI(EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo, EquipWeapons[CurrentWeaponIndex].Weapon->GunCurrentAmmo,!bCombatReady);
	if(bLineTraceFire) RifleLineTrace(CameraLocation, CameraRotation, IsMoving);
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
	if (!EquipWeapons[index].Weapon)
	{
		ChangeWeaponLock = false;
		return;
	}
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
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
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(Montage);
	if(Montage==FireMontage) SpawnBullet();
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
	DOREPLIFETIME(ACharacterBase, bIsPlaying);
}
#pragma endregion