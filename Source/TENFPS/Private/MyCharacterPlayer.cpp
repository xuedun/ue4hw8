// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterPlayer.h"
#include "WeaponBase.h"
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
}
