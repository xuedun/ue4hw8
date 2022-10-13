// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyAIController.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "EMyEnum.h"
#include "SMyStruct.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
class AMyAIControllerBot;
UCLASS()
class TENFPS_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMyGameModeBase();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void GameModeStartGame(int InEnemyNumPerMin,int InMaxAINum, int InMinPerBoss, int InRespawnNum);
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void EndGame();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePVEKill(AController* DamageCauser);
	void BotDeath(AMyAIControllerBot* AIC, AController* DamageCauser, FVector loc, FRotator rot);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateProtecteeHealthUI(float NewHealth);

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> PlayerPawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> BotPawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> ProtecteePawnClass;
	int8 CurrentAIIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AMyPlayerController*> AICs;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AMyAIControllerBot*> FreeAMyAIControllerArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AMyAIControllerBot*> AllAMyAIControllerArray;
	class AMyAIControllerProtectee* Protectee;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ProtecteeHealth = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UClass*> BonusList;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxAINum = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentAINum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int EnemyNumPerMin = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MinPerBoss = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int EnemyReserve = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SurEnemy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MinNum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bGameStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RespawnNum = 10;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DownTimeOut();

//	TArray<FSBuffInfo> BuffArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EBuffType, float> BuffMap;
	void GetBuff(EBuffType Buff,float BuffTime, AActor* BuffActor);
	UFUNCTION(BlueprintCallable)
		void UpdateBuffTime();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void EndBuff(EBuffType Buff);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void GameModeUpdateHealth(AController* Con, float NewHealth);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DamageUI(FVector Location,float Damage);
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void UpdateBossHealthUI(float NewHealth);
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		float BossHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasBoss = false;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateEnemyUI();

	UFUNCTION(BlueprintCallable)
		AMyAIControllerBot* CreateAIController();
	UFUNCTION(BlueprintCallable)
	void CreateABot();
	UFUNCTION(BlueprintCallable)
		void CreateABoss();
	UFUNCTION(BlueprintCallable)
		void CreateProtectee();

	void StartPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AActor*> AIPlayerStarts;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AActor*> ProtecteePlayerStarts;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AActor*> PlayerStarts;
	int SpawnIndex = 0;

	void PostLogin(APlayerController* NewPlayer) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AMyPlayerController*> PlayerControllerArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AMyPlayerState*> PlayerStateArray;
	

};

