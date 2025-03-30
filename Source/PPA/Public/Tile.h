// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"


UENUM()
enum class ETileStatus : uint8
{
	EMPTY     UMETA(DisplayName = "Empty"),
	OCCUPIED      UMETA(DisplayName = "Occupied"),
	OCCUPIED_BY_AI UMETA(DisplayName = "OccupiedByAi"),
	OCCUPIED_BY_HUMAN UMETA(DisplayName = "OccupiedByHuman")
};

UCLASS()
class PPA_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetHighlighted(bool bHighlighted);

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetHighlightedEnemy(bool bHighlighted);

	UFUNCTION(BlueprintCallable, Category = "Tile")
	FString GetTileID() const;

	// set the player owner and the status of a tile
	void SetTileStatus(const int32 TileOwner, const ETileStatus TileStatus);

	// get the tile status
	ETileStatus GetTileStatus();

	// get the tile owner
	int32 GetOwner();

	// set the (x, y) position
	void SetGridPosition(const double InX, const double InY);

	// get the (x, y) position
	FVector2D GetGridPosition();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Materials")
	UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Materials")
	UMaterialInterface* HighlightMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Materials")
	UMaterialInterface* HighlightAttackableEnemyMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETileStatus Status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PlayerOwner;

	// (x, y) position of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;

	//public:	
	//	// Called every frame
	//	virtual void Tick(float DeltaTime) override;

};
