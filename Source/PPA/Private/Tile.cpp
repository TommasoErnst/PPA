// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"


// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);


	Status = ETileStatus::EMPTY;
	PlayerOwner = -1;
	TileGridPosition = FVector2D(0, 0);

}

void ATile::SetTileStatus(const int32 TileOwner, const ETileStatus TileStatus)
{
	PlayerOwner = TileOwner;
	Status = TileStatus;
}

ETileStatus ATile::GetTileStatus()
{
	return Status;
}

int32 ATile::GetOwner()
{
	return PlayerOwner;
}

void ATile::SetGridPosition(const double InX, const double InY)
{
	TileGridPosition.Set(InX, InY);
}

FVector2D ATile::GetGridPosition()
{
	return TileGridPosition;
}


void ATile::SetHighlighted(bool bHighlighted)
{
	if (StaticMeshComponent)
	{
		if (bHighlighted && HighlightMaterial)
		{
			StaticMeshComponent->SetMaterial(0, HighlightMaterial);
		}
		else if (DefaultMaterial)
		{
			StaticMeshComponent->SetMaterial(0, DefaultMaterial);
		}
	}
}

void ATile::SetHighlightedEnemy(bool bHighlighted)
{
	if (StaticMeshComponent)
	{
		if (bHighlighted && HighlightAttackableEnemyMaterial)
		{
			StaticMeshComponent->SetMaterial(0, HighlightAttackableEnemyMaterial);
		}
		else if (DefaultMaterial)
		{
			StaticMeshComponent->SetMaterial(0, DefaultMaterial);
		}
	}
}

FString ATile::GetTileID() const
{
	
	TCHAR ColChar = 'A' + static_cast<int32>(TileGridPosition.Y);

	int32 RowNumber = static_cast<int32>(TileGridPosition.X) + 1;

	return FString::Printf(TEXT("%c%d"), ColChar, RowNumber);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
//void ATile::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}


