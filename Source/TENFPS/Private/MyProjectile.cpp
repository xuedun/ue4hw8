// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WeaponBase.h"
#include "CharacterBase.h"

// Sets default values
AMyProjectile::AMyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	RootComponent = Mesh;
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectile::OnHit);
	//ProjectileMovementComponent初始化
	ProjectileMovementComponent->ProjectileGravityScale = 0.3;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	InitialLifeSpan = 3.0f;

//	bReplicates = true;

}

void AMyProjectile::OnHit( class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit")));

	UGameplayStatics::ApplyPointDamage(SweepResult.GetActor(), Weapon->BaseDamage, -1 * ProjectileMovementComponent->Velocity.GetSafeNormal(), SweepResult, Character->GetController(), this, UDamageType::StaticClass());
//	this->Destroy();
}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();
	Weapon = Cast<AWeaponBase>(GetOwner());
	Character = Cast<ACharacterBase>(Weapon->GetOwner());
}

// Called every frame
void AMyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
