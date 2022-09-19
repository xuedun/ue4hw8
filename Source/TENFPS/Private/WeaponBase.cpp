// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetSimulatePhysics(true);

	bReplicates = true;
	if (AvailableFireMode.Num() > 0)
	{
		CurrentFireModeIndex = 0;
		CurrentFireMode = AvailableFireMode[0];
	}
	else
	{
		AvailableFireMode.Add(EFireMode::Semi_Auto);
		CurrentFireModeIndex = 0;
		CurrentFireMode = AvailableFireMode[0];
	}
}


void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (Character)
	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Equip Weapon Hit"));
		EquipWeapon();
		Character->EquipWeapon(this);
	}
}

//武器被拾取时候取消碰撞和模拟物理
void AWeaponBase::EquipWeapon()
{
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetSimulatePhysics(false);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FTransform SocketTransformA = WeaponMesh->GetSocketTransform(TEXT("Root"), RTS_World);
	FTransform SocketTransformB = WeaponMesh->GetSocketTransform(TEXT("Sight"), RTS_World);

	AimBaselineOffset = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT());
}

void AWeaponBase::FireAnimation()
{
	UAnimInstance* AnimInstance = this->WeaponMesh->GetAnimInstance();
	if(AnimInstance && WeaponFireMontage) AnimInstance->Montage_Play(WeaponFireMontage);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this,&AWeaponBase::OnSphereOverlap);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::MultiShootingEffect_Implementation()
{
	if (GetOwner() != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{

		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("Muzzle"),
			FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.1, 0.1, 0.1),
			EAttachLocation::KeepRelativeOffset, true, EPSCPoolMethod::None,
			true);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),FireSound,GetActorLocation());
	}
}

bool AWeaponBase::MultiShootingEffect_Validate()
{
	return true;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, GunCurrentAmmo);
	DOREPLIFETIME(AWeaponBase, ClipCurrentAmmo);
}