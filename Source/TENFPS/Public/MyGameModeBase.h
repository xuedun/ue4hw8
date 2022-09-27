// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyAIController.h"
#include "MyPlayerController.h"
#include "EMyEnum.h"
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
		void GameModeStartGame(int InEnemyNumPerMin,int InMaxAINum, int InMinPerBoss);
	UFUNCTION(BlueprintImplementableEvent)
		void EndGame();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePVEKill(AController* DamageCauser);
	void BotDeath(AMyAIControllerBot* AIC, AController* DamageCauser, FVector loc, FRotator rot);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateProtecteeHealthUI(float NewHealth);
	void StartPlay() override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> BotPawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> ProtecteePawnClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AActor*> AIPlayerStarts;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AActor*> ProtecteePlayerStarts;
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

protected:
	UFUNCTION(BlueprintCallable)
		AMyAIControllerBot* CreateAIController();
	UFUNCTION(BlueprintCallable)
	void CreateABot();
	UFUNCTION(BlueprintCallable)
		void CreateABoss();
	UFUNCTION(BlueprintCallable)
		void CreateProtectee();

};

