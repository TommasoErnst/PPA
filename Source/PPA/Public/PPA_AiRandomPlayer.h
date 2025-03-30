// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PPA_PlayerInterface.h"
#include "Tile.h"
#include "GameField.h"
#include "Sniper.h"
#include "PPA_AiRandomPlayer.generated.h"

UCLASS()
class PPA_API APPA_AiRandomPlayer : public APawn, public IPPA_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APPA_AiRandomPlayer();

	void SpawnUnit();
	void MoveToRandomTile();
	void MoveTowardClosestEnemy();
	void StartAiTurnLogic();
	void HandleNextUnit();
	void MoveAiUnitStepByStep();
	void AttackRandomEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Riferimenti ad oggetti
	UPROPERTY()
	class UPPA_GameInstance* GameInstance;

	UPROPERTY()
	class AGameField* GameField;

	UPROPERTY()
	class APPA_GameModeBase* GameMode;

	// Numero del giocatore
	UPROPERTY()
	int32 PlayerNumber = 1; // 1 = AI, 0 = Human

	// Array delle unità AI
	UPROPERTY()
	TArray<ABaseUnit*> Units;

	// Array delle unità umane
	UPROPERTY()
	TArray<ABaseUnit*> HumanUnitsArray;

	// Unità selezionata
	UPROPERTY()
	ABaseUnit* SelectedUnit;

	// Tipo di unità (serve solo in fase di spawn)
	int32 UnitType = -1;

	// Variabili per gestione dello spawn
	bool IsSniperSpawned = false;
	bool IsBrawlerSpawned = false;

	// Variabili/Oggetti di movimento e percorso
	UPROPERTY()
	TArray<ATile*> ReachableTiles;

	TArray<ATile*> SelectedPath;

	ATile* FinalPos;

	int32 CurrentStepIndex = 0;
	int32 CurrentUnitIndex = 0;

	// Timers
	FTimerHandle AITimerHandle;
	FTimerHandle MovementTimerHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;

};
