// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tile.h"
#include "BaseUnit.generated.h"

UCLASS()
class PPA_API ABaseUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	ATile* CurrentTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utils")
	int HeldByPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utils")
	bool bIsUnitMoved=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utils")
	bool bHasUnitAttacked=false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int MaxMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Health;

	

	// Imposta la tile corrente
	void SetIsUnitMoved(bool ismoved);

	bool GetIsUnitMoved() const;

	void SetHasUnitAttacked(bool hasattacked);

	bool GetHasUnitAttacked() const;

	// Imposta la tile corrente
	void SetCurrentTile(ATile* NewTile);

	// Ottieni la tile corrente
	ATile* GetCurrentTile() const;

	// Ottieni la posizione nella griglia
	FVector2D GetGridPosition() const;

	void SetHeldByPlayer(int32 PlayerNumber);

	int32 GetHeldByPlayer();

};
