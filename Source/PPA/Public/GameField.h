#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Obstacle.h"
#include "GameField.generated.h"

UCLASS()
class PPA_API AGameField : public AActor
{
	GENERATED_BODY()

public:
	/** Costanti */
	static const int32 NOT_ASSIGNED = -1;

	/** Proprietà configurabili */

	// Dimensione del campo
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	// Percentuale di ostacoli (es. 20%)
	UPROPERTY(EditDefaultsOnly, Category = "Obstacles")
	float ObstaclePercentage = 20.f;

	// Dimensione di una singola cella
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileSize;

	// Percentuale di padding tra le celle
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellPadding;

	// Moltiplicatore per calcolare la prossima posizione cella
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NextCellPositionMultiplier;

	//Classi di riferimento da inserire in blueprint

	// Classe Tile
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClass;

	// Classe Obstacle
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AObstacle> ObstacleClass;

	// Riferimenti vari

	// Array di tutte le tile
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	// Mappa da posizione a Tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// Mappa da posizione a Obstacle
	UPROPERTY(Transient)
	TMap<FVector2D, AObstacle*> ObstacleMap;

	// Metodi base e Costruttore

	AGameField();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	/** Logica di gioco */

	// Genera il campo di gioco
	void GenerateField();

	// Genera gli ostacoli
	void GenerateObstacles();

	// Restituisce le tile raggiungibili
	TArray<ATile*> GetReachableTiles(FVector2D StartPosition, int32 MaxDistance);

	// Restituisce le tile attaccabili
	TArray<ATile*> GetAttackableEnemies(FVector2D StartPosition, int32 AttackRange, bool bIsHuman);

	// Trova un percorso da start a end
	TArray<ATile*> FindPath(FVector2D Start, FVector2D End);

	// Trova un percorso con A*
	TArray<ATile*> FindPathAStar(const FVector2D& Start, const FVector2D& Goal);

	// Ottiene tutte le tile vuote
	TArray<ATile*> GetAllEmptyTiles();

	// Ottiene una tile da una posizione
	ATile* GetTileAtPosition(FVector2D GridPos);

	// Controlla se una posizione è valida
	inline bool IsValidPosition(const FVector2D Position) const;

	// Controlla se un ostacolo rompe la connettività della mappa
	bool DoesObstacleBreakConnectivity(FVector2D Position);

	// Controlla se la mappa è connessa
	bool IsMapConnected();

	// Ottiene i vicini di una posizione
	TArray<FVector2D> GetNeighbors(FVector2D Position);

	/** Utility */

	// Restituisce la posizione (X,Y) da un click
	FVector2D GetPosition(const FHitResult& Hit)
	{
		return Cast<ATile>(Hit.GetActor())->GetGridPosition();
	}

	// Restituisce l'array di tile
	TArray<ATile*>& GetTileArray();

	// Calcola posizione relativa da coordinate griglia
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// Calcola coordinate griglia da posizione relativa
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

private:
	/** Interno */

	// Set di posizioni con ostacoli (utile per pathfinding)
	TSet<FVector2D> Obstacles;
};

