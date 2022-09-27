// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDotProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
AMyDotProjectile::AMyDotProjectile()
{
	InitialLifeSpan = 20.0f;
	Tags.Add(TEXT("Dot"));
}

void AMyDotProjectile::OnHit(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("DotHit")));
	if (IgnoreList.Contains(OtherActor)) return;
	ACharacterBase* Char = Cast<ACharacterBase>(OtherActor);
	if (bDelay)
	{
		if (GetGameTimeSinceCreation() < 0.2f)
		{
			if (HasSameGroupTag(Cast<AController>(GetOwner())->GetPawn(), OtherActor)) return;
			if (Char)
			{
				Char->isPoisonous = true;
				Char->DamagePerSec += 5;
				IgnoreList.Add(OtherActor);
			}

		}
		else
			bDelay = false;
	}
	if (Char)
	{	
//		if ((Tags.Contains(TEXT("Player")) && OtherActor->Tags.Contains(TEXT("Player")))||Tags.Contains(TEXT("Attacker")) && OtherActor->Tags.Contains(TEXT("Attacker"))) return;
		if (HasSameGroupTag(this, OtherActor)) return;
		Char->isPoisonous = true;
		Char->DamagePerSec += 5;
		IgnoreList.Add(OtherActor);
	}
	
}

void AMyDotProjectile::StopOnHit(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("STopDotHit")));
	ACharacterBase* Char = Cast<ACharacterBase>(OtherActor);
	if (Char)
	{
		if (!IgnoreList.Contains(OtherActor)) return;
//		if (Tags.Contains(TEXT("Player")) && OtherActor->Tags.Contains(TEXT("Player"))) return;
		if (Tags.Contains(TEXT("Attacker")) && OtherActor->Tags.Contains(TEXT("Attacker"))) return;
		Char->isPoisonous = false;
		Char->DamagePerSec -= 5;
		IgnoreList.Remove(OtherActor);
//		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%i"), Char->DamagePerSec));
	}
}

void AMyDotProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AMyDotProjectile::OnHit);
	Mesh->OnComponentEndOverlap.AddDynamic(this, &AMyDotProjectile::StopOnHit);
}



