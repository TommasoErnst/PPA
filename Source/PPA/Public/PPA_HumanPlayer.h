// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PPA_GameInstance.h"
#include "PPA_PlayerInterface.h"
#include "GameField.h"
#include "Sniper.h"
#include "Brawler.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PPA_HumanPlayer.generated.h"

UCLASS()
class PPA_API APPA_HumanPlayer : public APawn, public IPPA_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APPA_HumanPlayer();

	// camera component attacched to player pawn
	UCameraComponent* Camera;

	// game instance reference
	UPPA_GameInstance* GameInstance;

	
	AGameField* GameField;

	UPROPERTY()
	class APPA_GameModeBase* GameMode;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnitType = -1;

	// keeps track of turn
	bool IsMyTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSniperSpawned = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBrawlerSpawned = false;


	bool IsTileHighlighted = false;

	//Previene la possibilità di cliccare un'altra unit/tile durante il movimento
	int32 AttackCounter = 0;

	int HeldBy = -1;


	int32 MinDmg = 0;


	int32 MaxDmg = 0;

	//Riferimento all'unità selezionata

	TArray<ABaseUnit*> UnitHumanArray;

	ABaseUnit* SelectedUnit = nullptr;

	ABaseUnit* OldSelectedUnit = nullptr;


	TArray<ATile*> ReachableTiles;


	TArray<ATile*> AttackableEnemies;


	TArray<ATile*> SelectedPath;


	int32 CurrentStepIndex;


	FTimerHandle MovementTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;



	UFUNCTION()
	void HandleSpawn(ATile* Tile);

	UFUNCTION()
	void HandleUnitSelection(ABaseUnit* Unit);

	UFUNCTION()
	void MoveUnitStepByStep();

	UFUNCTION()
	void MoveUnitToTile(ATile* Tile);

	// called on left mouse click (binding)
	UFUNCTION()
	void OnClick();

};
