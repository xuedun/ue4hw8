// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterProtectee.h"
#include "MyAIControllerProtectee.h"
#include "WeaponBase.h"
#include "MyGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
AMyCharacterProtectee::AMyCharacterProtectee()
{
	AIControllerClass = AMyAIControllerProtectee::StaticClass();
}

void AMyCharacterProtectee::OnHit(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (DamageCauser)
		if(DamageCauser->Tags.Contains(TEXT("Player"))) return;
	
	Health -= 1;
	AMyGameModeBase* Gamemode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	Gamemode->UpdateProtecteeHealthUI(Health);
	if (Health <= 0)
	{
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("GameEnd")));
		Gamemode->EndGame();
	}
}

void AMyCharacterProtectee::BeginPlay()
{
	Super::BeginPlay();
	OnTakePointDamage.AddDynamic(this, &AMyCharacterProtectee::OnHit);
	Health = 500;
	//	ServerChangeWeaponAnimation(0);
}

