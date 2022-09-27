// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBulletProjectile.h"

void AMyBulletProjectile::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectile::OnHit);
}
