// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyAIController.h"
#include "MyPlayerController.h"
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
	UFUNCTION(BlueprintImplementableEvent)
		void GameModeStartGame();
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AMyPlayerController*> AICs;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int MaxAINum;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AMyAIControllerBot*> FreeAMyAIControllerArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AMyAIControllerBot*> AllAMyAIControllerArray;
	class AMyAIControllerProtectee* Protectee;
protected:
	UFUNCTION(BlueprintCallable)
		AMyAIControllerBot* CreateAIController();
	UFUNCTION(BlueprintCallable)
	void CreateABot();
	UFUNCTION(BlueprintCallable)
		void CreateProtectee();

};

