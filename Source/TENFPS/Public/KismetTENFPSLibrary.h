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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PlayerKillNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PlayerDieNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PlayerHp;

};

UCLASS()
class TENFPS_API UKismetTENFPSLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
