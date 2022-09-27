// Fill out your copyright notice in the Description page of Project Settings.

#include "MyBuff.h"
#include "MyCharacterPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMyBuff::AMyBuff()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	bReplicates = true;

}

void AMyBuff::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	if (Cast<AMyCharacterPlayer>(OtherActor))
	{
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Buff")));
		AMyGameModeBase* Gamemode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		Gamemode->GetBuff(Bufftype, BuffTime,this);
	}	
}

// Called when the game starts or when spawned
void AMyBuff::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyBuff::OnSphereOverlap);
}

// Called every frame
void AMyBuff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

