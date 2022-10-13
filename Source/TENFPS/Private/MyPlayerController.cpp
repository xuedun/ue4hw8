// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyDamageType_Dot.h"
#include "CharacterBase.h"
#include "Blueprint/UserWidget.h"
#include "MyCharacterPlayer.h"

AMyPlayerController::AMyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> FindPlayerUI(TEXT("/Game/Blueprint/GameUI/W_PlayerUI"));
	PlayerUIClass = FindPlayerUI.Class;
}

void AMyPlayerController::UpdateDotDamage()
{
	if (!Chara) Chara = Cast<ACharacterBase>(GetPawn());
//	UGameplayStatics::ApplyDamage(Chara, Chara->DamagePerSec, nullptr, nullptr, UMyDamageType_Dot::StaticClass());
	UGameplayStatics::ApplyDamage(Chara, 10, nullptr, nullptr, UMyDamageType_Dot::StaticClass());
}

void AMyPlayerController::PlayerCameraShake(TSubclassOf<UCameraShakeBase> CameraShake)
{
	ClientPlayCameraShake(CameraShake, 1, ECameraShakePlaySpace::CameraLocal,FRotator::ZeroRotator);
}

void AMyPlayerController::CreatePlayerUI()
{
	if (PlayerUI)
	{
		PlayerUI->RemoveFromParent();
	}
	if (IsLocalPlayerController())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("PlayerUI")));
		PlayerUI = CreateWidget<UUserWidget>(GetWorld(), PlayerUIClass);
		if (PlayerUI)	
			PlayerUI->AddToViewport();
	}
}

void AMyPlayerController::SpawnPlayerCharacte(UClass* Class, FTransform SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
	PlayerCharacter = GetWorld()->SpawnActor<AMyCharacterPlayer>(Class,
		SpawnTransform,
		SpawnInfo);
	this->Possess(PlayerCharacter);

}


