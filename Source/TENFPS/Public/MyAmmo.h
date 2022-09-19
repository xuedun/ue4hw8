// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyAmmo.generated.h"

UCLASS()
class TENFPS_API AMyAmmo : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereCollision;
public:	
	// Sets default values for this actor's properties
	AMyAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
