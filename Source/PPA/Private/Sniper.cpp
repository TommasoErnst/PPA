// Fill out your copyright notice in the Description page of Project Settings.


#include "Sniper.h"

ASniper::ASniper()
{
	MaxMovement = 3;
	AttackRange = 10;
	MinDamage = 4;
	MaxDamage = 8;
	Health = 20;
	HeldByPlayer = 0;


	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);
}

void ASniper::BeginPlay()
{
	Super::BeginPlay();
}

void ASniper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
