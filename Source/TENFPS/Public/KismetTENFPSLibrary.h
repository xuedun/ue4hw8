// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KismetTENFPSLibrary.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FName PlayerName;
	UPROPERTY(BlueprintReadWrite)
		int32 PlayerKillNum;
	UPROPERTY(BlueprintReadWrite)
		int32 PlayerDieNum;
	FPlayerData()
	{
		PlayerName = TEXT("");
		PlayerDieNum = 0;
		PlayerKillNum = 0;
	}
};

UCLASS()
class TENFPS_API UKismetTENFPSLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
