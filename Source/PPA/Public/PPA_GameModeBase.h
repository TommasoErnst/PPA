// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PPA_GameInstance.h"
#include "GameField.h"
#include "BaseUnit.h"
#include "PPA_AiRandomPlayer.h"
#include "PPA_HumanPlayer.h"
#include "PPA_PlayerInterface.h"
#include "PPA_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PPA_API APPA_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Costruttore
	APPA_GameModeBase();

	bool IsGameOver = false;
	
	virtual void BeginPlay() override;

	// Metodi get per gli array dei Player
	UFUNCTION(BlueprintCallable, Category = "Units")
	const TArray<ABaseUnit*>& GetHumanUnits() const;

	UFUNCTION(BlueprintCallable, Category = "Units")
	const TArray<ABaseUnit*>& GetAiUnits() const;

	UFUNCTION(BlueprintCallable, Category = "Turn Log")
	void LogActionMessage(ABaseUnit* Unit, ATile* FromTile, ATile* ToTile = nullptr, int32 Damage = -1);

	// Mettodi per la gestione del game
	void ChoosePlayerAndStartGame();
	void TurnNextPlayer();
	int32 GetNextPlayer(int32 Player);
	void RemoveUnit(ABaseUnit* UnitToRemove);

	// Interazioni con le tile
	void SetHighlightedTileStatus(ABaseUnit* SelectedUnit, const TArray<ATile*>& ReachableTiles, const TArray<ATile*>& AttackableEnemies);
	ABaseUnit* SetTileUnit(const int32 PlayerNumber, const FVector& SpawnPosition, const int UnitType, ATile* Tile);

	void CheckWin();

protected:
	// Variabili di stato del gioco
	
	int32 CurrentPlayer = 0;
	int32 MoveCounter = 0;

	// Riferimenti e classi
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	UPPA_GameInstance* GameInstance;

	// Variabile per impostazione field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// Classi per l'estetica delle varie Unit
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> SniperHumanClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> SniperAiClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> BrawlerHumanClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> BrawlerAiClass;

	// Array delle unità
	UPROPERTY()
	TArray<ABaseUnit*> HumanUnits;

	UPROPERTY()
	TArray<ABaseUnit*> AiUnits;

	UPROPERTY()
	ABaseUnit* CurrentlySelectedUnit = nullptr;

	// Variabili e array di oggetti per la gestione delle celle evidenziate
	UPROPERTY()
	TArray<ATile*> OldReachableTiles;

	UPROPERTY()
	TArray<ATile*> OldAttackableEnemies;

	UPROPERTY()
	bool bIsHighlighted = false;

	// Players
	TArray<IPPA_PlayerInterface*> Players;
};
