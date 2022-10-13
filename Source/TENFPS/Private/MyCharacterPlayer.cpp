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

void AMyCharacterPlayer::ClientCreateUI_Implementation()
{
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("BeginPlay")));
	PlayerController = Cast<AMyPlayerController>(GetController());
	if(PlayerController) PlayerController->CreatePlayerUI();
}

void AMyCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &ACharacterBase::OnHit);
	OnTakeAnyDamage.AddDynamic(this, &ACharacterBase::OnHitAny);
	PlayerController = Cast<AMyPlayerController>(GetController());

	if (PlayerController) PlayerController->CreatePlayerUI();
	
//	ClientCreateUI();

	if (bStartWithWeapon) StartWithWeapon();//初始是否自带武器
//	if (bStartWithWeapon) StartWithWeapon();//初始是否自带武器
}

void AMyCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
