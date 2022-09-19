// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyCharacterProtectee.h"
#include "MyCharacterBot.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyAIControllerProtectee.h"
#include "MyAIControllerBot.h"
AMyGameModeBase::AMyGameModeBase()
{
	static ConstructorHelpers::FClassFinder<ACharacterBase> PlayerPawnOb(TEXT("/Game/Blueprint/BP_CharacterBase"));
	DefaultPawnClass = PlayerPawnOb.Class;
	PlayerControllerClass = AMyPlayerController::StaticClass();
	
	static ConstructorHelpers::FClassFinder<ACharacterBase> BotPawn(TEXT("Blueprint'/Game/Blueprint/Bot/BP_CharacterBot.BP_CharacterBot_C'"));
	BotPawnClass = BotPawn.Class;
	static ConstructorHelpers::FClassFinder<ACharacterBase> ProtecteePawn(TEXT("/Game/Blueprint/Protectee/BP_CharacterProtectee"));
	ProtecteePawnClass = ProtecteePawn.Class;
}

void AMyGameModeBase::StartPlay()
{
	Super::StartPlay();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("AIStart"), AIPlayerStarts);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("ProtecteePoint"), ProtecteePlayerStarts);
}

UClass* AMyGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AMyAIController>())
	{
		return BotPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AMyAIControllerBot* AMyGameModeBase::CreateAIController()
{
	FActorSpawnParameters Spawn;
	Spawn.Instigator = nullptr;
	Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMyAIControllerBot* AIC = GetWorld()->SpawnActor<AMyAIControllerBot>(Spawn);
	return AIC;
}

void AMyGameModeBase::CreateABot()
{
	AMyAIControllerBot* AIC;
	if (FreeAMyAIControllerArray.Num() > 0)
	{
		AIC = FreeAMyAIControllerArray[0];
		FreeAMyAIControllerArray.Remove(AIC);
	}
	else {
		AIC = CreateAIController();
		AllAMyAIControllerArray.Add(AIC);
	}
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = AIC;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UClass* BlueprintVar = StaticLoadClass(AMyCharacterBot::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Bot/BP_CharacterBot.BP_CharacterBot_C'"));
	int8 RespawnIndex = FMath::RandRange(0, AIPlayerStarts.Num() - 1);
	AMyCharacterBot* Bot = GetWorld()->SpawnActor<AMyCharacterBot>(BlueprintVar,
		AIPlayerStarts[RespawnIndex]->GetActorTransform(),
		SpawnInfo);
	AIC->Possess(Bot);
	AIC->Bot = Bot;
}

void AMyGameModeBase::CreateProtectee()
{
	if (Protectee) return;
	FActorSpawnParameters Spawn;
	Spawn.Instigator = nullptr;
	Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Protectee = GetWorld()->SpawnActor<AMyAIControllerProtectee>(Spawn);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = Protectee;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UClass* BlueprintVar = StaticLoadClass(AMyCharacterProtectee::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Protectee/BP_CharacterProtectee.BP_CharacterProtectee_C'"));
	TArray<AActor*> ProtecteeStarts;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("ProtecteePoint"), ProtecteeStarts);
	if (ProtecteeStarts.Num() == 0)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("未找到守护者出生点，创建守护者失败")));
		return;
	}
	int8 RespawnIndex = FMath::RandRange(0, ProtecteeStarts.Num() - 1);
	AMyCharacterProtectee* Bot = GetWorld()->SpawnActor<AMyCharacterProtectee>(BlueprintVar,
		ProtecteeStarts[RespawnIndex]->GetActorTransform(),
		SpawnInfo);
	Protectee->Possess(Bot);
	Protectee->Bot = Bot;
}

#pragma region ProtecteeAI


#pragma endregion