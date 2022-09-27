// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterBot.h"
#include "MyAIController.h"
#include "WeaponBase.h"
#include "MyAIControllerBot.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"
AMyCharacterBot::AMyCharacterBot()
{
	AIControllerClass = AMyAIController::StaticClass();
}

void AMyCharacterBot::OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("BotOnhit")));
	if (DamageCauser->Tags.Contains(TEXT("Attacker"))) return;
	if (DamageCauser->Tags.Contains(TEXT("Bullet"))) DamageCauser->Destroy();

	Health -= Damage;
	AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->DamageUI(GetActorLocation(), Damage);
	if (Tags.Contains(TEXT("Boss")))
	{
		GameMode->UpdateBossHealthUI(Health);
		if (!bHasPoisonous && Health < MaxBossHealth/2)
		{
			bHasPoisonous = true;
			PoisonousAttack();
		}
	}
	if (Health <= 0 && !bDead)
	{
		bDead = true;
		MulticastDieAnimation();
		PVEDeath(InstigatedBy);
	}
	else
	{
		AMyAIControllerBot* AIC = Cast<AMyAIControllerBot>(GetController());
		if(AIC)
		{
			if (AIC->bProTarget)
			{
	//			UKismetSystemLibrary::PrintString(GetWorld(), UKismetSystemLibrary::GetObjectName(InstigatedBy->GetPawn()));
				AIC->SetTarget(InstigatedBy->GetPawn());
				AIC->bProTarget = false;
			}
		}

	}
}

void AMyCharacterBot::PVEDeath(AController* DamageCauser)
{
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("DamageCauser Name : %s"), *DamageCauser->GetName()));
	AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	for (int i = 0;i < MaxWeaponNum;i++)
	{
		if (EquipWeapons[i].Weapon)
		{
			EquipWeapons[i].Weapon->Destroy();
		}
	}
	
	if (Tags.Contains(TEXT("Boss")))
		GameMode->EndGame();
	AMyAIControllerBot* DiePlayerController = Cast<AMyAIControllerBot>(GetController());
	Tags.Add(TEXT("Die"));
	
	GameMode->BotDeath(DiePlayerController, DamageCauser, GetActorLocation(),GetActorRotation());
	DiePlayerController->OnUnPossess();

}

void AMyCharacterBot::BeginPlay()
{
	Super::BeginPlay();
	if (Tags.Contains("Boss")) Health = MaxBossHealth;
	OnTakePointDamage.AddDynamic(this, &AMyCharacterBot::OnHit);
	if (FMath::FRand() > 0.5) StartWithWeapon();
	else BotType = AIType::Hook;
//	ServerChangeWeaponAnimation(0);
}
