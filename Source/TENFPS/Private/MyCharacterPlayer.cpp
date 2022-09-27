// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponBase.h"
#include "MyPlayerController.h"
void AMyCharacterPlayer::FireWeaponPrimary()
{
	Super::FireWeaponPrimary();
	if (bCombatReady)
	{
		//服务器端多播开火特效，开火动画，计算子弹减少，更新弹夹UI，进行射线检测
		//客户端准星扩散以及镜头抖动
		if (EquipWeapons[CurrentWeaponIndex].Weapon->ClipCurrentAmmo > 0)
		{
			ClientFire();
		}
	}
}

void AMyCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &ACharacterBase::OnHit);
	OnTakeAnyDamage.AddDynamic(this, &ACharacterBase::OnHitAny);
	PlayerController = Cast<AMyPlayerController>(GetController());

	if (PlayerController)
	{
		if (IsLocallyControlled())
		{
//			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("TREras1"));
			PlayerController->CreatePlayerUI();
		}
	}
 	else 
 	{
 		if (HasAuthority())
 		{
// 			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("TREras2"));
 			PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
 			if (PlayerController)
 			{
 				PlayerController->CreatePlayerUI();
			}
 		}
 	}
	if (bStartWithWeapon) StartWithWeapon();//初始是否自带武器
}

void AMyCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
