// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyCharacterProtectee.h"
#include "MyCharacterBot.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyAIControllerProtectee.h"
#include "MyAIControllerBot.h"
#include "MyPlayerState.h"
AMyGameModeBase::AMyGameModeBase()
{
	static ConstructorHelpers::FClassFinder<ACharacterBase> PlayerPawnOb(TEXT("/Game/Blueprint/BP_CharacterBase"));
	DefaultPawnClass = PlayerPawnOb.Class;
	PlayerControllerClass = AMyPlayerController::StaticClass();

//	static ConstructorHelpers::FClassFinder<ACharacterBase> PlayerPawn(TEXT("Blueprint'/Game/Blueprint/BP_CharacterBase.BP_CharacterBase_C'"));
//	PlayerPawnClass = PlayerPawn.Class;
	static ConstructorHelpers::FClassFinder<ACharacterBase> BotPawn(TEXT("Blueprint'/Game/Blueprint/Bot/BP_CharacterBot.BP_CharacterBot_C'"));
	BotPawnClass = BotPawn.Class;
	static ConstructorHelpers::FClassFinder<ACharacterBase> ProtecteePawn(TEXT("/Game/Blueprint/Protectee/BP_CharacterProtectee"));
	ProtecteePawnClass = ProtecteePawn.Class;

	BuffMap.Add(EBuffType::Bullet, 0);
	BuffMap.Add(EBuffType::Attack, 0);
	BuffMap.Add(EBuffType::Health, 0);
	BuffMap.Add(EBuffType::Defence, 0);
	BuffMap.Add(EBuffType::RPM, 0);
}

void AMyGameModeBase::BotDeath(AMyAIControllerBot* AIC, AController* DamageCauser,FVector loc, FRotator rot)
{
	FreeAMyAIControllerArray.Add(AIC);
	CurrentAINum -= 1;
	UpdatePVEKill(DamageCauser);

	int8 BonusListIndex = FMath::RandHelper(BonusList.Num()-1);
	auto& ite = BonusList[BonusListIndex];
	GetWorld()->SpawnActor(ite,&loc,&rot);

	SurEnemy--;
	if (SurEnemy == 0 && EnemyReserve == 0 &&!bHasBoss)
	{
		DownTimeOut();
	}
	UpdateEnemyUI();
}

UClass* AMyGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AMyAIController>())
	{
		return BotPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AMyGameModeBase::GetBuff(EBuffType Buff, float BuffTime, AActor* BuffActor)
{
	if (bGameStart) BuffActor->Destroy();
	BuffMap[Buff] = BuffTime;
	for (AMyPlayerController* Controller : AICs)
	{
		Controller->GetBuff(Buff, BuffTime);
		Cast<ACharacterBase>(Controller->GetPawn())->MulticastBuff(Buff,true);
	}

}



void AMyGameModeBase::UpdateBuffTime()
{
	for (auto& i : BuffMap)
	{
		if (i.Value == 1)
		{
			i.Value = 0;
			for (AMyPlayerController* Controller : AICs)
			{
				Cast<ACharacterBase>(Controller->GetPawn())->MulticastBuff(i.Key, false);
				Controller->EndBuff(i.Key);
			}
		}
		i.Value = i.Value == 0 ? 0 : i.Value - 1;
		
	}
	for (AMyPlayerController* Controller : AICs)
	{
		Controller->UpdateBuffUI();
	}
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
	int8 RespawnIndex = CurrentAIIndex++;
	CurrentAIIndex = CurrentAIIndex >= AIPlayerStarts.Num() ? 0 : CurrentAIIndex;
	AMyCharacterBot* Bot = GetWorld()->SpawnActor<AMyCharacterBot>(BlueprintVar,
		AIPlayerStarts[RespawnIndex]->GetActorTransform(),
		SpawnInfo);
	AIC->Possess(Bot);
	CurrentAINum += 1;
	EnemyReserve--;
	SurEnemy++;

}

void AMyGameModeBase::CreateABoss()
{
	AMyAIControllerBot* AIC;
	AIC = CreateAIController();
	AllAMyAIControllerArray.Add(AIC);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = AIC;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UClass* BlueprintVar = StaticLoadClass(AMyCharacterBot::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Bot/BP_CharacterBoss.BP_CharacterBoss_C'"));
	int8 RespawnIndex = CurrentAIIndex++;
	CurrentAIIndex = CurrentAIIndex >= AIPlayerStarts.Num() ? 0 : CurrentAIIndex;
	AMyCharacterBot* Bot = GetWorld()->SpawnActor<AMyCharacterBot>(BlueprintVar,
		AIPlayerStarts[RespawnIndex]->GetActorTransform(),
		SpawnInfo);
	AIC->Possess(Bot);
	bHasBoss = true;
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
}


void AMyGameModeBase::StartPlay()
{
	Super::StartPlay();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("AIStart"), AIPlayerStarts);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("ProtecteePoint"), ProtecteePlayerStarts);
}

void AMyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if(HasAuthority())
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("PlayerStart"), PlayerStarts);
	AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer);
	PlayerControllerArray.Add(PC);

// 	AMyPlayerState* PS = Cast<AMyPlayerState>(NewPlayer->PlayerState);
// 	PlayerStateArray.Add(PS);
// 	PS->Name = FText::FromString(GetNameSafe(PC));

	FLatentActionInfo LatentInfo;
	UKismetSystemLibrary::Delay(GetWorld(), 0.5, LatentInfo);
	PC->SpawnPlayerCharacte(PlayerPawnClass, PlayerStarts[SpawnIndex]->GetTransform());
	SpawnIndex = ++SpawnIndex >= PlayerStarts.Num() ? 0 : SpawnIndex;

}
