// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "MyCharacterPlayer.h"
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
	WeaponMesh->SetSimulatePhysics(false);

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
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Overlap")));
	AMyCharacterPlayer* Character = Cast<AMyCharacterPlayer>(OtherActor);
	if (Character&&Character->Is_Alive())
	{
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Equip")));
		Character->EquipWeapon(this);
		WeaponMesh->SetGenerateOverlapEvents(false);
		return;
	}
	if (OtherActor == GetOwner())
	{
		ACharacterBase* Chara = Cast<ACharacterBase>(OtherActor);
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Equip")));
		Chara->EquipWeapon(this);
	}
}

//武器被拾取时候取消碰撞和模拟物理并计算瞄准动作右手IK偏移
void AWeaponBase::EquipWeapon()
{
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetSimulatePhysics(false);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FTransform SocketTransformA = WeaponMesh->GetSocketTransform(TEXT("Root"), RTS_World);
	FTransform SocketTransformB = WeaponMesh->GetSocketTransform(TEXT("Sight"), RTS_World);

	AimBaselineOffset = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();
}

void AWeaponBase::FireAnimation(float MotagePlayRate)
{
	UAnimInstance* AnimInstance = this->WeaponMesh->GetAnimInstance();
	if(AnimInstance && WeaponFireMontage) AnimInstance->Montage_Play(WeaponFireMontage, MotagePlayRate);
}



void AWeaponBase::EndAnimation(class UAnimMontage* Montage)
{
	if (WeaponSingleMontage && Montage == WeaponSingleMontage)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
		if (Character)
		{
			Character->FireLock = false;
		}
	}
}

void AWeaponBase::MulticastAnimation_Implementation(class UAnimMontage* Montage)
{
	UAnimInstance* AnimInstance = this->WeaponMesh->GetAnimInstance();
	if (AnimInstance && WeaponSingleMontage) AnimInstance->Montage_Play(Montage);
	FOnMontageEnded BlendingOutDelegate;
	BlendingOutDelegate.BindUFunction(this, TEXT("EndAnimation"), Montage);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, Montage);
}

bool AWeaponBase::MulticastAnimation_Validate(class UAnimMontage* Montage)
{
	return true;
}

void AWeaponBase::ShootingEffect()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("Muzzle"),
		FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.1, 0.1, 0.1),
		EAttachLocation::KeepRelativeOffset, true, EPSCPoolMethod::None,
		true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
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


void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, ClipCurrentAmmo);
}