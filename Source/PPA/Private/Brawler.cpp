// Fill out your copyright notice in the Description page of Project Settings.


#include "Brawler.h"

ABrawler::ABrawler()
{
	MaxMovement = 6;
	AttackRange = 1;
	MinDamage = 1;
	MaxDamage = 6;
	Health = 40;
	HeldByPlayer = 0;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);
}

void ABrawler::BeginPlay()
{
	Super::BeginPlay();
	// Eventuale codice...
}

void ABrawler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Eventuale codice...
}