// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterBot.h"
#include "MyAIController.h"
#include "WeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
AMyCharacterBot::AMyCharacterBot()
{
	AIControllerClass = AMyAIController::StaticClass();
}

void AMyCharacterBot::OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (DamageCauser->Tags.Contains(TEXT("Attacker"))) return;

	Health -= Damage;
	if (Health <= 0 && !bDead)
	{
		bDead = true;
		MulticastDieAnimation();
		PVEDeath(DamageCauser);
	}
}

void AMyCharacterBot::PVEDeath(AActor* DamageCauser)
{
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon)
		{
			EquipWeapons[i].Weapon->Destroy();
		}
	}
	AMyAIController* DiePlayerController = Cast<AMyAIController>(GetController());
	if (DiePlayerController)
	{
		DiePlayerController->PVEDeath(DamageCauser);
	}
}

void AMyCharacterBot::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &AMyCharacterBot::OnHit);
//	ServerChangeWeaponAnimation(0);
}
