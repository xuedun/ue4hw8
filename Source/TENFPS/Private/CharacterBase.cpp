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
#include "MyDotProjectile.h"
#include "MyAIControllerBot.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "MyDamageType_Dot.h"
#include "Blueprint/UserWidget.h"
//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
#pragma region Component
	bIsPlaying = false;
	bDead = false;
	bCombatReady = false;
	bADS = false;

	// 胶囊体碰撞
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 移动鼠标只影响相机旋转不直接控制角色旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 角色移动
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// 相机
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

#pragma endregion

#pragma region Weapon
	//初始化枪械栏位
	MaxWeaponNum = 3;
	FSWeaponPanelInfo temp = { TEXT("HosterBRSocket"),nullptr,EWeaponType::TwoHandWeapon};
	EquipWeapons.Add(temp);
	temp = { TEXT("HosterBLSocket"),nullptr,EWeaponType::TwoHandWeapon};
	EquipWeapons.Add(temp);
	temp = { TEXT("HosterThighRSocket"),nullptr,EWeaponType::OneHandWeapon};
	EquipWeapons.Add(temp);

#pragma endregion

	BuffMap.Add(EBuffType::Bullet, false);
	BuffMap.Add(EBuffType::Attack, false);
	BuffMap.Add(EBuffType::Health, false);
	BuffMap.Add(EBuffType::Defence, false);
	BuffMap.Add(EBuffType::RPM, false);

	BulletNum.Add(EBulletType::AR, 60);
	BulletNum.Add(EBulletType::ShotGun, 30);
	BulletNum.Add(EBulletType::Pistol, 30);
	BulletNum.Add(EBulletType::Snaper, 30);
}



#pragma region Engine
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AMyPlayerController>(GetController());
	if(PlayerController) PlayerController->Chara = this;
	AnimInstance = GetMesh()->GetAnimInstance();
//	OnTakePointDamage.AddDynamic(this,&ACharacterBase::OnHit);
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator temp = GetBaseAimRotation() - K2_GetActorRotation();
	Yaw = temp.Yaw;
	Pitch = temp.Pitch;
	Ani_Speed = GetVelocity().Size();
	Ani_Direction = AnimInstance->CalculateDirection(GetVelocity(), GetActorRotation());

	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	bLeftIK = bCombatReady && (CurrentWeaponType == EWeaponType::TwoHandWeapon) && (AnimInstance->GetCurveValue("LeftHandIKOff") == 0);

	if (GetCurrentWeapon())
	{
		if (GetCurrentWeapon()->CurrentFireMode == EFireMode::Single)
		{
			FTransform GripTransform = GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("GripL"), ERelativeTransformSpace::RTS_World);
			GetMesh()->TransformToBoneSpace(TEXT("hand_r"), GripTransform.GetLocation(), FRotator::ZeroRotator, GripLOffset, GripLRotator);
		}
	}

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

	PlayerInputComponent->BindAction("WeaponR", IE_Pressed, this, &ACharacterBase::WeaponR);
	PlayerInputComponent->BindAction("WeaponL", IE_Pressed, this, &ACharacterBase::WeaponL);
	PlayerInputComponent->BindAction("WeaponT", IE_Pressed, this, &ACharacterBase::WeaponT);
	PlayerInputComponent->BindAction("ChangeToNextWeapon", IE_Pressed, this, &ACharacterBase::ChangeToNextWeapon);
	PlayerInputComponent->BindAction("ChangeToLastWeapon", IE_Pressed, this, &ACharacterBase::ChangeToLastWeapon);

	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &ACharacterBase::ChangeFireMode);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &ACharacterBase::ADS);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &ACharacterBase::StopADS);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::InputFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::InputFireReleased);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACharacterBase::Reload);

	PlayerInputComponent->BindAction("SpeedUP", IE_Pressed, this, &ACharacterBase::SpeedUp);
	PlayerInputComponent->BindAction("SpeedUP", IE_Released, this, &ACharacterBase::StopSpeedUp);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ACharacterBase::ThrowWeapon);
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

void ACharacterBase::SpeedUp()
{
//	if(!bCombatReady) GetCharacterMovement()->MaxWalkSpeed += 200.0f;
}

void ACharacterBase::StopSpeedUp()
{
//	if(!bCombatReady) GetCharacterMovement()->MaxWalkSpeed -= 200.0f;
}
#pragma endregion

#pragma region Weapon
//未设置初始武器则默认初始自动步枪
void ACharacterBase::StartWithWeapon()
{
	if (HasAuthority())
	{
		int8 StartWeaponIndex = 0;
		if (StartWeaponList.Num() > 0)
		{
			StartWeaponIndex = FMath::RandHelper(StartWeaponList.Num() - 1);
			PurchaseWeapon(StartWeaponList[StartWeaponIndex]);
		}
		else
			PurchaseWeapon(StaticLoadClass(AWeaponBase::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/BP_AR.BP_AR_C'")));
		return;
	}
}

void ACharacterBase::PurchaseWeapon(UClass* Class)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(Class,
		GetActorTransform(),
		SpawnInfo);
}

void ACharacterBase::EquipWeapon(AWeaponBase* Weapon)
{
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		FSWeaponPanelInfo& WeaponSlot = EquipWeapons[i];
		if (!WeaponSlot.Weapon && WeaponSlot.WeaponType == Weapon->WeaponType)
		{
			WeaponSlot.Weapon = Weapon;
			if (!bCombatReady) CurrentWeaponIndex = i;
			Weapon->SetOwner(this);
			Weapon->EquipWeapon();
			Weapon->K2_AttachToComponent(this->GetMesh(), WeaponSlot.SocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			break;
		}
	}
}

AWeaponBase* ACharacterBase::GetCurrentWeapon()
{
	if (bCombatReady) return EquipWeapons[CurrentWeaponIndex].Weapon;
	else return nullptr;
}

void ACharacterBase::ChangeFireMode()
{
	if (!GetCurrentWeapon()) return;
	if (GetCurrentWeapon()->AvailableFireMode.Num() == 0) return;
	GetCurrentWeapon()->CurrentFireModeIndex = GetCurrentWeapon()->CurrentFireModeIndex + 1 >= GetCurrentWeapon()->AvailableFireMode.Num() ? 0 : GetCurrentWeapon()->CurrentFireModeIndex + 1;
	GetCurrentWeapon()->CurrentFireMode = GetCurrentWeapon()->AvailableFireMode[GetCurrentWeapon()->CurrentFireModeIndex];
	ClientWeaponUI();
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

#pragma endregion

#pragma region Base
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

void ACharacterBase::ServerUpdataYP_Implementation(float P, float Y)
{
	MulticastUpdataYP(P, Y);
}

bool ACharacterBase::ServerUpdataYP_Validate(float P, float Y)
{
	return true;
}

void ACharacterBase::MulticastUpdataYP_Implementation(float P, float Y)
{
	if (!IsLocallyControlled())
	{
		Pitch = P;
		Yaw = Y;
	}
}

bool ACharacterBase::MulticastUpdataYP_Validate(float P, float Y)
{
	return true;
}

//值复制
void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, bADS);
	DOREPLIFETIME(ACharacterBase, bCombatReady);
	DOREPLIFETIME(ACharacterBase, bIsPlaying);

}
#pragma endregion

#pragma region ADS

void ACharacterBase::ADS()
{
	if (!GetCurrentWeapon()) return;
	bADS = true;
	bUseControllerRotationYaw = true;
	CameraBoom->SocketOffset.X = CameraBoom->SocketOffset.X + 100;
	ServerChangeADSState(true);
	//狙击枪开镜
	if (GetCurrentWeapon()->WeaponIndex == 4)
	{
		this->SetActorHiddenInGame(true);
		for (auto WeaponSlot : EquipWeapons)
		{
			if (WeaponSlot.Weapon)
			{
				WeaponSlot.Weapon->SetActorHiddenInGame(true);
			}
		}
		if (GetFollowCamera())
			GetFollowCamera()->SetFieldOfView(FieldOfAimingView);
		WidgetScope = CreateWidget<UUserWidget>(GetWorld(), SniperScopeBPClass);
		if (WidgetScope)
			WidgetScope->AddToViewport();
	}
	return;
}

void ACharacterBase::StopADS()
{
	if (!bCombatReady) return;
	if (!bADS) return;
	bADS = false;
	bUseControllerRotationYaw = false;
	CameraBoom->SocketOffset.X = CameraBoom->SocketOffset.X - 100;
	//	CameraBoom->SocketOffset = CameraBoom->SocketOffset - CameraBoom->GetForwardVector() * 100;
	ServerChangeADSState(false);
	if (GetCurrentWeapon()->WeaponIndex == 4)
	{
		this->SetActorHiddenInGame(false);
		for (auto WeaponSlot : EquipWeapons)
		{
			if (WeaponSlot.Weapon)
			{
				WeaponSlot.Weapon->SetActorHiddenInGame(false);
			}
		}
		if (GetFollowCamera())
			GetFollowCamera()->SetFieldOfView(90);
		if (WidgetScope)
			WidgetScope->RemoveFromParent();
	}
	return;
}

void ACharacterBase::ServerChangeADSState_Implementation(bool bNextADS)
{
	bADS = bNextADS;
	bUseControllerRotationYaw = bNextADS;
}

bool ACharacterBase::ServerChangeADSState_Validate(bool bNextADS)
{
	return true;
}

#pragma endregion

#pragma region Health
//点状伤害（hook、bullet）
void ACharacterBase::OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	//死亡或伤害来自友军不计算
	if (bDead || GetController() == InstigatedBy) return;
	if (DamageCauser)
	{
		if (bIsPlaying && DamageCauser->Tags.Contains(TEXT("Player")) && this->Tags.Contains(TEXT("Player"))) return;
		if (DamageCauser->Tags.Contains(TEXT("Attacker")) && this->Tags.Contains(TEXT("Attacker"))) return;
		if (DamageCauser->Tags.Contains(TEXT("Destory")) && !DamageCauser->Tags.Contains(TEXT("Penetrate"))) DamageCauser->Destroy();
	}
	Health -= BuffMap[EBuffType::Defence] ? Damage / 2 : Damage;
	ServerDeath(InstigatedBy);
}

void ACharacterBase::OnHitAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bDead||InstigatedBy || DamageCauser) return;
	Health -= DamagePerSec;
	if (BuffMap[EBuffType::Health]) Health += 10;
	Health = Health > 100 ? 100 : Health;
	ServerDeath(InstigatedBy);
}

void ACharacterBase::ServerDeath(class AController* InstigatedBy)
{
	ClientUpdateHealthUI(Health);
	Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->GameModeUpdateHealth(GetController(), Health);

	if (!PlayerController) PlayerController = Cast<AMyPlayerController>(GetController());
	if (Health <= 0 && !bDead)
	{
		if (bIsPlaying)
		{
			PlayerController->PVEDeath();
		}
		else
		{
			PlayerController->PVPDeath(InstigatedBy->GetPawn());
		}
		//死亡
		bDead = true;
		MulticastDieAnimation();
	}
}



void ACharacterBase::ClientUpdateHealthUI_Implementation(float NewHealth)
{
	if (PlayerController)
	{
		PlayerController->UpdateHealthUI(NewHealth);
	}
}

//多播死亡动画
void ACharacterBase::MulticastDieAnimation_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool ACharacterBase::MulticastDieAnimation_Validate()
{
	return true;
}

bool ACharacterBase::Is_Alive()
{
	return !bDead;
}

void ACharacterBase::ClientRespawn_Implementation()
{
	//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ClientRespawn")));
	AMyPlayerController* DiePlayerController = Cast<AMyPlayerController>(GetController());
	//	UKismetSystemLibrary::PrintString(GetWorld(), UKismetSystemLibrary::GetObjectName(DiePlayerController));
	if (DiePlayerController)
	{
		DiePlayerController->RespawnDownTime();
	}
}

#pragma endregion 

#pragma region Reload

void ACharacterBase::Reload()
{
	if (ReloadLock) return;
	if ((!GetCurrentWeapon()) || BulletNum[GetCurrentWeapon()->BulletType] <= 0 || GetCurrentWeapon()->ClipCurrentAmmo >= GetCurrentWeapon()->MaxClipAmmo) return;
	ReloadLock = true;
	ServerReload();
}

void ACharacterBase::ServerReload_Implementation()
{
	if (GetCurrentWeapon()->IsSingleReload)
	{
		GetCurrentWeapon()->MulticastAnimation(GetCurrentWeapon()->WeaponReloadMontage);
		MulticastPlayAnimation(GetCurrentWeapon()->CharacterReloadMontage);
	}
	else
	{
		if (GetCurrentWeapon()->CharacterReloadMontage)
			MulticastPlayAnimation(GetCurrentWeapon()->CharacterReloadMontage);
		else MulticastPlayAnimation(ReloadMontage);
		if (BulletNum[GetCurrentWeapon()->BulletType] + GetCurrentWeapon()->ClipCurrentAmmo > GetCurrentWeapon()->MaxClipAmmo)
		{
			BulletNum[GetCurrentWeapon()->BulletType] = BulletNum[GetCurrentWeapon()->BulletType] + GetCurrentWeapon()->ClipCurrentAmmo - GetCurrentWeapon()->MaxClipAmmo;
			GetCurrentWeapon()->ClipCurrentAmmo = GetCurrentWeapon()->MaxClipAmmo;
		}
		else
		{
			GetCurrentWeapon()->ClipCurrentAmmo += BulletNum[GetCurrentWeapon()->BulletType];
			BulletNum[GetCurrentWeapon()->BulletType] = 0;
		}
	}
}

bool ACharacterBase::ServerReload_Validate()
{
	return true;
}

void ACharacterBase::HideClip()
{
	if (GetCurrentWeapon())
		if (GetCurrentWeapon()->WeaponIndex == 1)
			GetCurrentWeapon()->WeaponMesh->HideBoneByName(TEXT("Magazine"), EPhysBodyOp::PBO_None);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (GetCurrentWeapon()->Ammo)
	{
		Ammo = GetWorld()->SpawnActor<AMyAmmo>(GetCurrentWeapon()->Ammo,
			GetMesh()->GetSocketTransform(TEXT("hand_lClipSocket"), ERelativeTransformSpace::RTS_World),
			SpawnInfo);
		Ammo->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lClipSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		bHasAmmo = true;
	}
	else bHasAmmo = false;
}

void ACharacterBase::DropClip()
{
	if (bHasAmmo)
	{
		Ammo->Mesh->K2_DetachFromComponent(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
		Ammo->Mesh->SetEnableGravity(true);
		Ammo->Mesh->SetSimulatePhysics(true);
		Ammo = nullptr;
		bHasAmmo = false;
	}
}

void ACharacterBase::GetNewClip()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (GetCurrentWeapon()->Ammo)
	{
		Ammo = GetWorld()->SpawnActor<AMyAmmo>(GetCurrentWeapon()->Ammo,
			GetMesh()->GetSocketTransform(TEXT("hand_lClipSocket"), ERelativeTransformSpace::RTS_World),
			SpawnInfo);
		Ammo->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lClipSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		bHasAmmo = true;
	}
	else bHasAmmo = false;
}

void ACharacterBase::UnhideClip()
{
	if (bHasAmmo)
	{
		Ammo->K2_DestroyActor();
		Ammo = nullptr;
		bHasAmmo = false;
	}
	if (GetCurrentWeapon())
		if (GetCurrentWeapon()->WeaponIndex == 1)
			GetCurrentWeapon()->WeaponMesh->UnHideBoneByName(TEXT("Magazine"));
	ClientUpdateAmmoUI(GetCurrentWeapon()->ClipCurrentAmmo, BulletNum[GetCurrentWeapon()->BulletType], !bCombatReady);
	ReloadLock = false;
}

void ACharacterBase::AmmoCheck()
{
	if (HasAuthority())
	{
		if (BulletNum[GetCurrentWeapon()->BulletType] > 0)
		{
			BulletNum[GetCurrentWeapon()->BulletType]--;
			GetCurrentWeapon()->ClipCurrentAmmo++;
		}
		if (BulletNum[GetCurrentWeapon()->BulletType] <= 0 || GetCurrentWeapon()->ClipCurrentAmmo == GetCurrentWeapon()->MaxClipAmmo)
		{
			ReloadLock = false;
			MulticastEndSingleReloadAnimation();
		}
	}
	ClientUpdateAmmoUI(GetCurrentWeapon()->ClipCurrentAmmo, BulletNum[GetCurrentWeapon()->BulletType], !bCombatReady);
}

void ACharacterBase::ClientUpdateAmmoUI_Implementation(int32 ClipCurrentAmmo, int32 GunCurrentAmmo, bool HideAmmoUI)
{
	if (PlayerController)
	{
		PlayerController->UpdateAmmoUI(ClipCurrentAmmo, GunCurrentAmmo, HideAmmoUI);
	}
}

void ACharacterBase::MulticastEndSingleReloadAnimation_Implementation()
{
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_JumpToSection(TEXT("End"), GetCurrentWeapon()->CharacterReloadMontage);
	GetCurrentWeapon()->WeaponMesh->GetAnimInstance()->Montage_JumpToSection(TEXT("End"), GetCurrentWeapon()->WeaponReloadMontage);
	AnimInstance->Montage_JumpToSection(TEXT("End"), GetCurrentWeapon()->CharacterReloadMontage);

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUFunction(this, TEXT("SingleAnimationAfterReload"));
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, GetCurrentWeapon()->CharacterReloadMontage);
}

bool ACharacterBase::MulticastEndSingleReloadAnimation_Validate()
{
	return true;
}

void ACharacterBase::SingleAnimationAfterReload()
{
	GetCurrentWeapon()->WeaponMesh->GetAnimInstance()->Montage_Play(GetCurrentWeapon()->WeaponSingleMontage);
}


#pragma endregion

#pragma region Fire

void ACharacterBase::InputFirePressed()
{
	//上一次开火以及换武器换弹动画未结束时无法进行开火。
	if (FireLock || ChangeWeaponLock || ReloadLock) return;
	isFiring = true;
	FireLock = true;
	FireWeaponPrimary();
}

void ACharacterBase::InputFireReleased()
{
	isFiring = false;
	BurstNum = 0;
	//重置连发后坐力
	RecoilXCoordPerShoot = 0;
	OldVerticalRecoilAmount = 0;
	NewVerticalRecoilAmount = 0;
	VerticalRecoilAmount = 0;
	OldHorizontalRecoilAmount = 0;
	NewHorizontalRecoilAmount = 0;
	HorizontalRecoilAmount = 0;
}

void ACharacterBase::FireWeaponPrimary()
{
	if (!GetCurrentWeapon()) {
		//未装备武器时默认近战攻击
		ServerHook();
	}
	else
		//装备武器且子弹足够时进行服务器开火。
		//服务器端多播开火特效，开火动画，计算子弹减少，更新弹夹UI，进行射线检测（可选子弹抛体命中计算伤害或是射线检测计算伤害）
		//客户端准星扩散以及镜头抖动（仅在玩家人物中触发
		if (GetCurrentWeapon()->ClipCurrentAmmo > 0)
		{
			ServerFireRifleWeapon(GetFollowCamera()->GetComponentLocation(), GetFollowCamera()->GetComponentRotation(), false);
			ClientRecoil();
		}
		else
		{
			FireLock = false;
		}
}

//客户端准星扩散以及镜头抖动以及后坐力
void ACharacterBase::ClientFire_Implementation()
{
	if (GetCurrentWeapon())
	{
		if (!PlayerController) PlayerController = Cast<AMyPlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->PlayerCameraShake(EquipWeapons[CurrentWeaponIndex].Weapon->CameraShakeClass);
			PlayerController->DoCrosshairRecoil();
		}
	}
}

//后坐力
void ACharacterBase::ClientRecoil_Implementation()
{
	UCurveFloat* VerticalRecoilCurve = nullptr;
	UCurveFloat* HorizontalRecoilCurve = nullptr;
	if (GetCurrentWeapon())
	{
		VerticalRecoilCurve = GetCurrentWeapon()->VerticalRecoilCurve;
		HorizontalRecoilCurve = GetCurrentWeapon()->HorizontalRecoilCurve;
	}
	RecoilXCoordPerShoot += 0.1;
	if (VerticalRecoilCurve)
	{
		NewVerticalRecoilAmount = VerticalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot);
		NewHorizontalRecoilAmount = HorizontalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot);
	}
	VerticalRecoilAmount = NewVerticalRecoilAmount - OldVerticalRecoilAmount;
	HorizontalRecoilAmount = NewHorizontalRecoilAmount - OldHorizontalRecoilAmount;
	if (PlayerController)
	{
		FRotator ControllerRotator = GetControlRotation();
		PlayerController->SetControlRotation(FRotator(ControllerRotator.Pitch + VerticalRecoilAmount,
			ControllerRotator.Yaw + HorizontalRecoilAmount,
			ControllerRotator.Roll));
	}
	OldVerticalRecoilAmount = NewVerticalRecoilAmount;
}

//服务器开火
void ACharacterBase::ServerFireRifleWeapon_Implementation(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	//服务器端多播开火特效，开火动画，计算子弹减少，更新弹夹UI，进行射线检测
	//	EquipWeapons[CurrentWeaponIndex].Weapon->MultiShootingEffect();
	if (!GetCurrentWeapon()) return;
	//子弹减少
	if (!Tags.Contains(TEXT("Bot")))
		if (!BuffMap[EBuffType::Bullet])
			GetCurrentWeapon()->ClipCurrentAmmo -= 1;
	//多播武器开火
	CurrentWeaponType = GetCurrentWeapon()->WeaponType;
	if (CurrentWeaponType == EWeaponType::OneHandWeapon)
	{
		if (bADS) MulticastFireAnimation(OneHandADSFireMontage);
		else MulticastFireAnimation(OneHandHipFireMontage);
	}
	else
	{
		if(GetCurrentWeapon()->CharacterFireMontage)
			MulticastFireAnimation(GetCurrentWeapon()->CharacterFireMontage);
		else MulticastFireAnimation(CharacterFireMontage);
	}

	//射线检测
	if (bLineTraceFire) RifleLineTrace(CameraLocation, CameraRotation, IsMoving);
	else ClientSpawnBullet();
}

bool ACharacterBase::ServerFireRifleWeapon_Validate(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	return true;
}

void ACharacterBase::ClientSpawnBullet_Implementation()
{
	if (!GetCurrentWeapon()) return;
	if (IsLocallyControlled())
	{
		FVector Start = GetCurrentWeapon()->WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		FVector Velocity = (UpdateLookAtPoint() - Start);
		FVector SpreadVelocity;
		//子弹速度向量方向从枪口指向屏幕中心
		Velocity = ((Yaw >= 90 && Yaw <= 180) || (Yaw <= -90 && Yaw >= -180) || (Yaw >= -270 && Yaw <= -180) || (Yaw <= 270 && Yaw >= 180)) ? GetCurrentWeapon()->WeaponMesh->GetSocketRotation(TEXT("Muzzle")).Vector() : Velocity;
		FTransform SpawnTransForm = GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("Muzzle"), ERelativeTransformSpace::RTS_World);
		//枪口散布
		for (int i = 0;i < GetCurrentWeapon()->PelletPerShot;i++)
		{
			float Angle = PI / (0.5f) * FMath::FRand();
			float Displacement;
			if (bADS)
			{
				//狙击枪开镜无散射
				if (GetCurrentWeapon()->WeaponIndex == 4)
					Displacement = FMath::Tan(PI / (180.f) * 0) * GetCurrentWeapon()->BulletDistance * FMath::FRand();
				//霰弹枪瞄准散布角度减半
				if (GetCurrentWeapon()->WeaponIndex == 2)
					Displacement = FMath::Tan(PI / (180.f) * GetCurrentWeapon()->MaxSpreadAngle / 2) * GetCurrentWeapon()->BulletDistance * FMath::FRand();
			}
			else Displacement = FMath::Tan(PI / (180.f) * GetCurrentWeapon()->MaxSpreadAngle) * GetCurrentWeapon()->BulletDistance * FMath::FRand();
			float X = Displacement * FMath::Cos(Angle);
			float Y = Displacement * FMath::Sin(Angle);
			SpreadVelocity = Velocity.ToOrientationRotator().Vector() * GetCurrentWeapon()->BulletDistance +
				Y * FRotationMatrix(Velocity.ToOrientationRotator()).GetScaledAxis(EAxis::Y) +
				X * FRotationMatrix(Velocity.ToOrientationRotator()).GetScaledAxis(EAxis::Z);
			FVector EndLocation = Start + SpreadVelocity;
			TArray<AActor*> IgnoreArray;
			FHitResult OutHit;
			IgnoreArray.Add(this);
			//bool HitSucess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
			//				IgnoreArray, EDrawDebugTrace::Persistent, OutHit, true);
			ServerSpawnBullet(GetCurrentWeapon()->Bullet, SpawnTransForm, SpreadVelocity);
		}
		//狙击枪瞄准开火自动关瞄准镜
		if (bADS && (GetCurrentWeapon()->WeaponIndex == 4))
			StopADS();
	}
}

void ACharacterBase::ServerSpawnBullet_Implementation(UClass* Class, FTransform SpawnTransForm, FVector Velocity)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = GetController();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	Bullet = GetWorld()->SpawnActor<AMyProjectile>(Class,
		SpawnTransForm,
		SpawnInfo);
	if (Bullet)
	{
		if (GetCurrentWeapon()) Bullet->ProjectileMovementComponent->Velocity = Velocity.GetSafeNormal() * GetCurrentWeapon()->BulletVelocity;
		else Bullet->ProjectileMovementComponent->Velocity = Velocity.GetSafeNormal() * HadokenVelocity;
	}

	if (Tags.Contains("Player"))
	{
		Bullet->Tags.Add("Player");
	}
	if (Tags.Contains("Attacker"))
	{
		Bullet->Tags.Add("Attacker");
	}

	if (GetCurrentWeapon()) Bullet->Damage = BuffMap[EBuffType::Attack] ? GetCurrentWeapon()->BaseDamage * 1.5 : GetCurrentWeapon()->BaseDamage;
	else Bullet->Damage = BuffMap[EBuffType::Attack] ? HadokenBaseDamege * 1.5 : HadokenBaseDamege;
	Bullet = nullptr;
}

bool ACharacterBase::ServerSpawnBullet_Validate(UClass* Class, FTransform SpawnTransForm, FVector Velocity)
{
	return true;
}

void ACharacterBase::MulticastFireAnimation_Implementation(class UAnimMontage* Montage)
{
	if (!GetCurrentWeapon()) return;
	float m_RPM = BuffMap[EBuffType::RPM] ? GetCurrentWeapon()->RPM * 1.5 : GetCurrentWeapon()->RPM;
	//多播人物开火动画、武器开火动画、武器枪火粒子效果
	GetCurrentWeapon()->FireAnimation(m_RPM / 200);
	GetCurrentWeapon()->ShootingEffect();

	AnimInstance->Montage_Play(Montage, m_RPM / 200);
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUFunction(this, TEXT("FireEndDelegate"), Montage);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
	return;
}

bool ACharacterBase::MulticastFireAnimation_Validate(class UAnimMontage* Montage)
{
	return true;
}

void ACharacterBase::FireEndDelegate()
{
	if(!GetCurrentWeapon())
	{
		UnlockAfterAnimation();
		return;
	}
	if (IsLocallyControlled())
	{
		switch (GetCurrentWeapon()->CurrentFireMode)
		{
		case EFireMode::Full_Auto:
			if (isFiring)
			{
				FireWeaponPrimary();
				return;
			}
			break;
		case EFireMode::Burst:
			BurstNum += 1;
			if (isFiring && BurstNum < 3)
			{
				FireWeaponPrimary();
				return;
			}
			break;
		case EFireMode::Single:
			//single开火模式额外播放拉栓动画
			if (GetCurrentWeapon()->WeaponSingleMontage)
			{
				GetCurrentWeapon()->MulticastAnimation(GetCurrentWeapon()->WeaponSingleMontage);
				return;
			}
			break;
		}
	}
	UnlockAfterAnimation();
}

void ACharacterBase::RifleLineTrace(FVector CameraLocation, FRotator CameraRotation, bool IsMoving)
{
	if (!GetCurrentWeapon()) return;
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
		if (GetCurrentWeapon()) EndLocation = CameraLocation + CameraForwardVector * GetCurrentWeapon()->BulletDistance;
	}
	bool HitSucess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::None, OutHit, true);
	if (HitSucess)
	{
		ACharacterBase* HitCharacter = Cast<ACharacterBase>(OutHit.Actor);
		if (HitCharacter)
		{
			//命中玩家
			DamagePlayer(OutHit.PhysMaterial.Get(), OutHit.Actor.Get(), CameraForwardVector, OutHit);
		}
		else
		{

		}
	}
}

void ACharacterBase::DamagePlayer(UPhysicalMaterial* PhysicalMaterial, AActor* DamagedActor, FVector& HitFromDirection, FHitResult& HitInfo)
{
	float Damage;
	float BaseDamage;
	if (!GetCurrentWeapon()) return;
	if (GetCurrentWeapon()) BaseDamage = GetCurrentWeapon()->BaseDamage;
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
	UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, HitFromDirection, HitInfo, GetController(), this, UDamageType::StaticClass());
}

#pragma endregion 

#pragma region SimpleAttack
//毒雾攻击
void ACharacterBase::PoisonousAttack()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	FireLock = true;
	ServerPoisonous();
	FTransform SpawnTransfrom = GetActorTransform();
	SpawnTransfrom.SetScale3D(FVector::FVector(20, 20, 1));
	ServerSpawnBullet(PoisonousFrog, SpawnTransfrom, FVector::ZeroVector);
}

void ACharacterBase::ServerPoisonous_Implementation()
{
	MulticastPlayAnimation(PoisonousMontage);
}

bool ACharacterBase::ServerPoisonous_Validate()
{
	return true;
}

//空手远程攻击
void ACharacterBase::HadokenAttack()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	FireLock = true;
	ServerHadoken();
}

void ACharacterBase::ServerHadoken_Implementation()
{
	MulticastPlayAnimation(HadokenMontage);
}

bool ACharacterBase::ServerHadoken_Validate()
{
	return true;
}

void ACharacterBase::Hadoken()
{
	if (IsLocallyControlled())
	{
		FVector Start = GetMesh()->GetSocketLocation(TEXT("Hadoken"));
		FVector Velocity = (UpdateLookAtPoint() - Start);
		//子弹速度向量方向从枪口指向屏幕中心
//		Ftemp = GetBaseAimRotation() - K2_GetActorRotation();
		Velocity = ((Yaw >= 90 && Yaw <= 180) || (Yaw <= -90 && Yaw >= -180) || (Yaw >= -270 && Yaw <= -180) || (Yaw <= 270 && Yaw >= 180)) ? GetMesh()->GetSocketRotation(TEXT("Hadoken")).Vector() : Velocity;
		FTransform SpawnTransForm = GetMesh()->GetSocketTransform(TEXT("Hadoken"), ERelativeTransformSpace::RTS_World);

		ServerSpawnBullet(HadokenBullet, SpawnTransForm, Velocity);
		FireLock = false;
	}
}

//空手近战攻击
void ACharacterBase::Hook()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	FireLock = true;
	ServerHook();
}

void ACharacterBase::ServerHook_Implementation()
{
	MulticastPlayAnimation(HookMontage);
}

bool ACharacterBase::ServerHook_Validate()
{
	return true;
}

//三连击
void ACharacterBase::ThreeHookHit()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	FireLock = true;
	ServerThreeHookHit();
}

void ACharacterBase::ServerThreeHookHit_Implementation()
{
	MulticastPlayAnimation(ThreeHookHitMontage);
}

bool ACharacterBase::ServerThreeHookHit_Validate()
{
	return true;
}

//空手近战攻击伤害判定
void ACharacterBase::StartHookTrace()
{
	StartLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
}

void ACharacterBase::EndHookTrace()
{
	if (!HasAuthority()) return;
	TArray<AActor*> IgnoreArray;
	TArray<FHitResult> OutHit;
	IgnoreArray.Add(this);
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon) IgnoreArray.Add(EquipWeapons[i].Weapon);
	}
	FVector EndLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLocation, EndLocation, 20, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::None, OutHit, true);
	TArray<AActor*> HitArray;
	for (auto HitResult : OutHit)
	{
		if (HitArray.Contains(HitResult.Actor.Get())) return;
		HitArray.Add(HitResult.Actor.Get());
		UGameplayStatics::ApplyPointDamage(HitResult.Actor.Get(), BuffMap[EBuffType::Attack] ? HookBaseDamage * 1.5 : HookBaseDamage, -1 * HitResult.ImpactNormal, HitResult, GetController(), this, UDamageType::StaticClass());
	}
}

void ACharacterBase::StartHookLTrace()
{
	StartLocation = GetMesh()->GetSocketLocation(TEXT("hand_l"));
}

void ACharacterBase::EndHookLTrace()
{
	if (!HasAuthority()) return;
	TArray<AActor*> IgnoreArray;
	TArray<FHitResult> OutHit;
	IgnoreArray.Add(this);
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon) IgnoreArray.Add(EquipWeapons[i].Weapon);
	}
	FVector EndLocation = GetMesh()->GetSocketLocation(TEXT("hand_l"));
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLocation, EndLocation, 20, ETraceTypeQuery::TraceTypeQuery1, false,
		IgnoreArray, EDrawDebugTrace::None, OutHit, true);
	for (auto HitResult : OutHit)
	{
		TArray<AActor*> HitArray;
		HitArray.Add(HitResult.Actor.Get());
		UGameplayStatics::ApplyPointDamage(HitResult.Actor.Get(), BuffMap[EBuffType::Attack] ? HookBaseDamage * 1.5 : HookBaseDamage, -1 * HitResult.ImpactNormal, HitResult, GetController(), this, UDamageType::StaticClass());
	}
}

#pragma endregion

#pragma region Buff
void ACharacterBase::MulticastBuff_Implementation(EBuffType Buff,bool bBuffState)
{
	BuffMap[Buff] = bBuffState;
}

bool ACharacterBase::MulticastBuff_Validate(EBuffType Buff, bool bBuffState)
{
	return true;
}

void ACharacterBase::GetAmmoBullet()
{
	BulletNum[EBulletType::AR] += 100;
	BulletNum[EBulletType::ShotGun] += 30;
	BulletNum[EBulletType::Pistol] += 30;
	if (GetCurrentWeapon())
		ClientUpdateAmmoUI(GetCurrentWeapon()->ClipCurrentAmmo, BulletNum[GetCurrentWeapon()->BulletType], !bCombatReady);
}
#pragma endregion

#pragma region ThrowWeapon
void ACharacterBase::ThrowWeapon()
{
	ServerThrowWeapon();
}

void ACharacterBase::ServerThrowWeapon_Implementation()
{
	if (GetCurrentWeapon())
	{
		MulticastThrowWeapon();
		ClientWeaponUI();
		ClientUpdateAmmoUI(0, 0, true);
	}
//	GetCurrentWeapon()->Destroy();
}

bool ACharacterBase::ServerThrowWeapon_Validate()
{
	return true;
}

void ACharacterBase::MulticastThrowWeapon_Implementation()
{
	bCombatReady = false;
	if(EquipWeapons[CurrentWeaponIndex].Weapon)
		EquipWeapons[CurrentWeaponIndex].Weapon->K2_DetachFromActor();
	EquipWeapons[CurrentWeaponIndex].Weapon = nullptr;
}

bool ACharacterBase::MulticastThrowWeapon_Validate()
{
	return true;
}
#pragma endregion

#pragma region ChangeWeapon

void ACharacterBase::WeaponR()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	ChangeWeaponLock = true;
	ServerChangeWeaponAnimation(0);
	return;
}

void ACharacterBase::WeaponL()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	ChangeWeaponLock = true;
	ServerChangeWeaponAnimation(1);
	return;
}

void ACharacterBase::WeaponT()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	ChangeWeaponLock = true;
	ServerChangeWeaponAnimation(2);
	return;
}

void ACharacterBase::ChangeToNextWeapon()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	ChangeWeaponLock = true;
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	//若没有装备武器且当前栏位正好有武器则装备武器
	if (GetCurrentWeapon() && !bCombatReady)
	{
		ServerChangeWeaponAnimation(CurrentWeaponIndex);
		return;
	}

	int len = MaxWeaponNum;
	int i = (CurrentWeaponIndex + 1) % len;
	while (i != CurrentWeaponIndex)
	{
		if (EquipWeapons[i].Weapon)
		{
			ServerChangeWeaponAnimation(i);
			return;
		}
		i = (i + 1) % len;
	}
}

void ACharacterBase::ChangeToLastWeapon()
{
	if (ChangeWeaponLock || ReloadLock || FireLock) return;
	ChangeWeaponLock = true;
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	if (GetCurrentWeapon() && !bCombatReady)
	{
		ServerChangeWeaponAnimation(CurrentWeaponIndex);
		return;
	}
	int len = MaxWeaponNum;
	int i = CurrentWeaponIndex == 0 ? len - 1 : CurrentWeaponIndex - 1;
	while (i != CurrentWeaponIndex)
	{
		if (EquipWeapons[i].Weapon)
		{
			ServerChangeWeaponAnimation(i);
			return;
		}
		i = i == 0 ? len - 1 : i - 1;
	}
}

//RPC
void ACharacterBase::ServerChangeWeaponAnimation_Implementation(int8 index)
{
	if (!EquipWeapons[index].Weapon)
	{
		ChangeWeaponLock = false;
		return;
	}
	MulticastChangeWeaponAnimation(index);
}

bool ACharacterBase::ServerChangeWeaponAnimation_Validate(int8 index)
{
	return true;
}

void ACharacterBase::MulticastChangeWeaponAnimation_Implementation(int8 index)
{
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	//收枪
	if (index == CurrentWeaponIndex && bCombatReady)
	{
		TargetWeaponIndex = index;
		CurrentWeaponIndex = index;
		bCombatReady = false;

		AnimInstance->Montage_Play(HolsterWeaponMontages[index]);
		FOnMontageBlendingOutStarted MontageBlendingOutDelegate;
		MontageBlendingOutDelegate.BindUFunction(this, TEXT("UnlockAfterAnimation"));
		AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendingOutDelegate, HolsterWeaponMontages[index]);
		return;
	}
	//拿枪
	if (!bCombatReady)
	{
		TargetWeaponIndex = index;
		CurrentWeaponIndex = index;
		bCombatReady = true;

		AnimInstance->Montage_Play(EquipWeaponMontages[index]);
		FOnMontageBlendingOutStarted MontageBlendingOutDelegate;
		MontageBlendingOutDelegate.BindUFunction(this, TEXT("UnlockAfterAnimation"));
		AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendingOutDelegate, EquipWeaponMontages[index]);
		return;
	}

	//换枪
	if (index != CurrentWeaponIndex && bCombatReady)
	{
		TargetWeaponIndex = index;
		bCombatReady = true;

		AnimInstance->Montage_Play(HolsterWeaponMontages[CurrentWeaponIndex]);

		FOnMontageBlendingOutStarted MontageBlendingOutDelegate;
		MontageBlendingOutDelegate.BindUFunction(this, TEXT("EquipWeaponAnimation"));
		AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendingOutDelegate, HolsterWeaponMontages[CurrentWeaponIndex]);
		return;
	}
}

bool ACharacterBase::MulticastChangeWeaponAnimation_Validate(int8 index)
{
	return true;
}

//委托
void ACharacterBase::EquipWeaponAnimation()
{
	CurrentWeaponIndex = TargetWeaponIndex;
	AnimInstance->Montage_Play(EquipWeaponMontages[CurrentWeaponIndex]);

	FOnMontageBlendingOutStarted MontageBlendingOutDelegate;
	MontageBlendingOutDelegate.BindUFunction(this, TEXT("UnlockAfterAnimation"));
	AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendingOutDelegate, EquipWeaponMontages[CurrentWeaponIndex]);
}



//蒙太奇骨骼通知
void ACharacterBase::GrabWeaponR()
{
	if(GetCurrentWeapon())
		GetCurrentWeapon()->K2_AttachToComponent(this->GetMesh(), TEXT("hand_rSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	CurrentWeaponType = EquipWeapons[CurrentWeaponIndex].WeaponType;
//IK计算偏移
	if (GetCurrentWeapon())
	{
		FTransform GripTransform = GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("GripL"), ERelativeTransformSpace::RTS_World);
		GetMesh()->TransformToBoneSpace(TEXT("hand_r"), GripTransform.GetLocation(), FRotator::ZeroRotator, GripLOffset, GripLRotator);
		FTransform GripRTransform = GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("Sight"), ERelativeTransformSpace::RTS_World);
		GetMesh()->TransformToBoneSpace(TEXT("hand_r"), GripRTransform.GetLocation(), FRotator::ZeroRotator, AimBaseLineOffset, AimBaseLineRotator);
		FTransform MagTransform = GetCurrentWeapon()->WeaponMesh->GetSocketTransform(TEXT("Mag"), ERelativeTransformSpace::RTS_World);
		GetMesh()->TransformToBoneSpace(TEXT("hand_r"), MagTransform.GetLocation(), FRotator::ZeroRotator, MagOffset, MagRotator);
	}
}

void ACharacterBase::OnHolster()
{
	if (EquipWeapons[CurrentWeaponIndex].Weapon)
		EquipWeapons[CurrentWeaponIndex].Weapon->K2_AttachToComponent(this->GetMesh(), EquipWeapons[CurrentWeaponIndex].SocketName, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
}

void ACharacterBase::GrabWeaponLTemp()
{
	if (GetCurrentWeapon())
		GetCurrentWeapon()->K2_AttachToComponent(this->GetMesh(), TEXT("hand_lSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
}

//武器界面UI
void ACharacterBase::ClientWeaponUI_Implementation()
{
	if (!Tags.Contains(TEXT("Player"))) return;
	int index = 0;
	FName Text;
	if (GetCurrentWeapon())
	{
		index = GetCurrentWeapon()->WeaponIndex;
		switch (GetCurrentWeapon()->CurrentFireMode)
		{
		case EFireMode::Semi_Auto:
			Text = FName(TEXT("半自动"));
			break;
		case EFireMode::Full_Auto:
			Text = FName(TEXT("全自动"));
			break;
		case EFireMode::Burst:
			Text = FName(TEXT("三连发"));
			break;
		case EFireMode::Single:
			Text = FName(TEXT("单发"));
		}
	}
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
	if(PC)
		PC->UpdateWeaponUI(index, Text);
}

#pragma endregion

#pragma region Animation
//多播动画
void ACharacterBase::MulticastPlayAnimation_Implementation(class UAnimMontage* Montage)
{
	if (!AnimInstance) AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->Montage_Play(Montage, BuffMap[EBuffType::RPM] ? 1.5 : 1);
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUFunction(this, TEXT("UnlockAfterAnimation"), Montage);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
}

bool ACharacterBase::MulticastPlayAnimation_Validate(class UAnimMontage* Montage)
{
	return true;
}

void ACharacterBase::UnlockAfterAnimation()
{
	ChangeWeaponLock = false;
	FireLock = false;
	ReloadLock = false;
	BurstNum = 0;
	ClientWeaponUI();
	if(GetCurrentWeapon())
		ClientUpdateAmmoUI(GetCurrentWeapon()->ClipCurrentAmmo, BulletNum[GetCurrentWeapon()->BulletType], !bCombatReady);
	else
		ClientUpdateAmmoUI(0, 0, !bCombatReady);
}
#pragma endregion


