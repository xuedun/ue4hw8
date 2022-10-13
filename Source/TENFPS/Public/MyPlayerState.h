// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TENFPS_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	FText Name;
	float Score = 0;
	float Die = 0;
	float Health = 100;

};
