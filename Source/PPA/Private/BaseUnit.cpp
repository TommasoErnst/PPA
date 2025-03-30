// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"

// Sets default values
ABaseUnit::ABaseUnit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseUnit::SetIsUnitMoved(bool ismoved)
{
	bIsUnitMoved = ismoved;
}

bool ABaseUnit::GetIsUnitMoved() const
{
	return bIsUnitMoved;
}

void ABaseUnit::SetHasUnitAttacked(bool hasattacked)
{
	this->bHasUnitAttacked = hasattacked;
	
}

bool ABaseUnit::GetHasUnitAttacked() const
{
	return bHasUnitAttacked;
}

void ABaseUnit::SetCurrentTile(ATile* NewTile)
{
	CurrentTile = NewTile;
}

ATile* ABaseUnit::GetCurrentTile() const
{
	return CurrentTile;
}

FVector2D ABaseUnit::GetGridPosition() const
{
	if (CurrentTile)
	{
		return CurrentTile->GetGridPosition();
	}
	return FVector2D(-1, -1); // Valore di fallback se non assegnata
}

void ABaseUnit::SetHeldByPlayer(int32 PlayerNumber)
{
	HeldByPlayer = PlayerNumber;
}

int32 ABaseUnit::GetHeldByPlayer()
{
	return HeldByPlayer;
}

