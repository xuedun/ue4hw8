// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WeaponBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
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

	//ProjectileMovementComponent初始化
	ProjectileMovementComponent->ProjectileGravityScale = 0.3;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	InitialLifeSpan = 3.0f;

	bReplicates = true;
//	Tags.Add(TEXT("Bullet"));
}

void AMyProjectile::OnHit( class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ProHit")));
	if (HasAuthority())
	{
		//投射物对于每个单位只造成一次伤害
		if (IgnoreList.Contains(SweepResult.GetActor())) return;
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit")));
		if(SweepResult.PhysMaterial.Get())
		{ 
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit")));
			switch (SweepResult.PhysMaterial.Get()->SurfaceType)
			{
			case EPhysicalSurface::SurfaceType1:
			{
				Damage = Damage * 4;
			}
			break;
			case EPhysicalSurface::SurfaceType2:
			{
				Damage = Damage * 1;
			}
			break;
			case EPhysicalSurface::SurfaceType3:
			{
				Damage = Damage * 0.8;
			}
			break;
			case EPhysicalSurface::SurfaceType4:
			{
				Damage = Damage * 0.6;
			}
			break;
			}
		}

		UGameplayStatics::ApplyPointDamage(SweepResult.GetActor(), Damage, -1 * ProjectileMovementComponent->Velocity.GetSafeNormal(), SweepResult, Cast<AController>(GetOwner()), this, UDamageType::StaticClass());
		IgnoreList.Add(SweepResult.GetActor());
		if(Cast<ACharacterBase>(SweepResult.GetActor()))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, SweepResult.Location, FRotator::ZeroRotator, FVector::OneVector, true, EPSCPoolMethod::None, true);
	}
//	this->Destroy();
}

bool AMyProjectile::HasSameGroupTag(AActor* A1, AActor* A2)
{
	return (A1->Tags.Contains(TEXT("Player")) && A2->Tags.Contains(TEXT("Player"))) || A1->Tags.Contains(TEXT("Attacker")) && A2->Tags.Contains(TEXT("Attacker"));
}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();
//	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectile::OnHit);
}

// Called every frame
void AMyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
