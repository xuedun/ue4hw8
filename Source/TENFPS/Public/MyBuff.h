// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EMyEnum.h"
#include "MyBuff.generated.h"

UCLASS()
class TENFPS_API AMyBuff : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AMyBuff();
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere)
	EBuffType Bufftype;
	UPROPERTY(EditAnywhere)
	float BuffTime;
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
