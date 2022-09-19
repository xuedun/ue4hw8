// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAmmo.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
// Sets default values
AMyAmmo::AMyAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AmmoMesh"));
	RootComponent = Mesh;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);

	Mesh->SetEnableGravity(false);
	Mesh->SetSimulatePhysics(false);

//	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMyAmmo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

