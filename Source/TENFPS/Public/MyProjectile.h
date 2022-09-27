// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterBase.h"
#include "MyProjectile.generated.h"

UCLASS()
class TENFPS_API AMyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComponent;
	AMyProjectile();
	UFUNCTION()
		virtual void OnHit(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* EmitterTemplate;
	float Damage;
	TArray<AActor*> IgnoreList;
	bool HasSameGroupTag(AActor* A1,AActor* A2);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
