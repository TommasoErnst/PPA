// Fill out your copyright notice in the Description page of Project Settings.


#include "GameField.h"

AGameField::AGameField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// size of the field (3x3)
	Size = 25;
	// tile dimension
	TileSize = 120.f;
	// tile padding percentage 
	CellPadding = 0.2f;


}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tilepadding
	NextCellPositionMultiplier = (TileSize + TileSize * CellPadding) / TileSize;
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();
	GenerateObstacles();
	for (const auto& Elem : TileMap)
	{
		FVector2D TilePosition = Elem.Key;
		ATile* Tile = Elem.Value;

		/*
		<<< DEBUG PER LE TILE GENERATE COME OCCUPIED >>>

		if (Tile && Tile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			// Log in console
			UE_LOG(LogTemp, Warning, TEXT("Tile Occupied at X=%d, Y=%d"), (int32)TilePosition.X, (int32)TilePosition.Y);
		}

		*/
	}
}


// Metodo che genera il field
void AGameField::GenerateField()
{
	for (int32 IndexX = 0; IndexX < Size; IndexX++)
	{
		for (int32 IndexY = 0; IndexY < Size; IndexY++)
		{
			FVector Location = AGameField::GetRelativeLocationByXYPosition(IndexX, IndexY);
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);
			const float TileScale = TileSize / 100.f;
			const float Zscaling = 0.2f;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, Zscaling));
			Obj->SetGridPosition(IndexX, IndexY);
			TileArray.Add(Obj);
			TileMap.Add(FVector2D(IndexX, IndexY), Obj);
		}
	}
}


void AGameField::GenerateObstacles()
{
	int32 TotalCells = Size * Size;
	int32 NumObstacles = FMath::RoundToInt((ObstaclePercentage / 100.f) * TotalCells);

	TArray<FVector2D> AvailablePositions;

	for (int32 IndexX = 0; IndexX < Size; IndexX++)
	{
		for (int32 IndexY = 0; IndexY < Size; IndexY++)
		{
			AvailablePositions.Add(FVector2D(IndexX, IndexY));
		}
	}

	for (int32 i = AvailablePositions.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		AvailablePositions.Swap(i, j);
	}

	int32 ObstaclesPlaced = 0;

	for (FVector2D Position : AvailablePositions)
	{
		if (ObstaclesPlaced >= NumObstacles) break;

		FVector Location = GetRelativeLocationByXYPosition(Position.X, Position.Y);

		// Verifica se l'ostacolo rompe la connettività prima di posizionarlo
		if (!DoesObstacleBreakConnectivity(Position))
		{
			AObstacle* Obstacle = GetWorld()->SpawnActor<AObstacle>(ObstacleClass, Location, FRotator::ZeroRotator);
			if (Obstacle)
			{
				ObstacleMap.Add(Position, Obstacle);
				// Trova la tile corrispondente usando GetXYPositionByRelativeLocation
				FVector2D TilePosition = GetXYPositionByRelativeLocation(Location);

				// Cerca la tile nella tua struttura dati
				 // TileMap è la mappa delle tile
				if (TileMap.Contains(TilePosition)) 
				{
					ATile* Tile = TileMap[TilePosition];
					if (Tile)
					{
						Tile->SetTileStatus(-1, ETileStatus::OCCUPIED);
					}
				}
				ObstaclesPlaced++;
			}
		}
	}
}

bool AGameField::DoesObstacleBreakConnectivity(FVector2D Position)
{
	// Temporaneamente piazza l'ostacolo per il test
	ObstacleMap.Add(Position, nullptr);

	// Controlla la connettività
	bool bIsConnected = IsMapConnected();

	// Rimuove l'ostacolo temporaneo
	ObstacleMap.Remove(Position);

	// Se la mappa non è connessa, allora l'ostacolo la rompe
	return !bIsConnected;  
}


// Metodo che controlla la connessione della mappa
bool AGameField::IsMapConnected()
{
	TArray<FVector2D> Visited;
	TArray<FVector2D> Queue;

	FVector2D StartPosition = FVector2D(0, 0);

	Queue.Add(StartPosition);
	Visited.Add(StartPosition);

	while (Queue.Num() > 0)
	{
		FVector2D Current = Queue[0];
		Queue.RemoveAt(0);

		TArray<FVector2D> Neighbors = GetNeighbors(Current);

		for (FVector2D Neighbor : Neighbors)
		{
			if (Neighbor.X >= 0 && Neighbor.X < Size &&
				Neighbor.Y >= 0 && Neighbor.Y < Size &&
				!ObstacleMap.Contains(Neighbor) && !Visited.Contains(Neighbor))
			{
				Queue.Add(Neighbor);
				Visited.Add(Neighbor);
			}
		}
	}

	// Controlliamo che tutte le celle libere siano state visitate
	for (int32 IndexX = 0; IndexX < Size; IndexX++)
	{
		for (int32 IndexY = 0; IndexY < Size; IndexY++)
		{
			FVector2D Position = FVector2D(IndexX, IndexY);
			if (!ObstacleMap.Contains(Position) && !Visited.Contains(Position))
			{
				// La mappa non è connessa
				return false;
			}
		}
	}

	return true;
}

// Restituisce i vicini della posizione data (sinistra, destra, sopra, sotto)
TArray<FVector2D> AGameField::GetNeighbors(FVector2D Position)
{
	TArray<FVector2D> Neighbors;

	if (Position.X > 0) Neighbors.Add(FVector2D(Position.X - 1, Position.Y)); // Sinistra
	if (Position.X < Size - 1) Neighbors.Add(FVector2D(Position.X + 1, Position.Y)); // Destra
	if (Position.Y > 0) Neighbors.Add(FVector2D(Position.X, Position.Y - 1)); // Sopra
	if (Position.Y < Size - 1) Neighbors.Add(FVector2D(Position.X, Position.Y + 1)); // Sotto

	return Neighbors;
}


// Ritorna l'array delle tile
TArray<ATile*>& AGameField::GetTileArray()
{
	return TileArray;
}


FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * NextCellPositionMultiplier * FVector(InX, InY, 0);
}


FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double XPos = Location.X / (TileSize * NextCellPositionMultiplier);
	const double YPos = Location.Y / (TileSize * NextCellPositionMultiplier);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), XPos, YPos));
	return FVector2D(XPos, YPos);
}


// Verifica che la posizione sia valida
inline bool AGameField::IsValidPosition(const FVector2D Position) const
{
	return 0 <= Position.X && Position.X < Size && 0 <= Position.Y && Position.Y < Size;
}

 // Restituisce la ile alla posizione x,y
ATile* AGameField::GetTileAtPosition(FVector2D GridPos)
{
	for (ATile* Tile : TileArray)
	{
		FVector2D GridPosition = Tile->GetGridPosition();
		if (Tile && GridPosition == GridPos)
		{
			return Tile;
		}
	}
	return nullptr;
}

// Restituisce le celle raggiungibili dalla posizione iniziale, tenendo conto degli ostacoli
TArray<ATile*> AGameField::GetReachableTiles(FVector2D StartPosition, int32 MaxDistance)
{
	TArray<ATile*> ReachableTiles;
	TSet<FVector2D> Visited;
	TQueue<TPair<FVector2D, int32>> Queue;

	Queue.Enqueue(TPair<FVector2D, int32>(StartPosition, 0));
	Visited.Add(StartPosition);

	while (!Queue.IsEmpty())
	{
		TPair<FVector2D, int32> Current;
		Queue.Dequeue(Current);

		FVector2D CurrentPosition = Current.Key;
		int32 CurrentDistance = Current.Value;

		if (TileMap.Contains(CurrentPosition))
		{
			ATile* CurrentTile = TileMap[CurrentPosition];
			if (CurrentTile)
			{
				// Aggiunge la tile solo se è libera
				if (CurrentTile->GetTileStatus() != ETileStatus::OCCUPIED &&
					CurrentTile->GetTileStatus() != ETileStatus::OCCUPIED_BY_HUMAN &&
					CurrentTile->GetTileStatus() != ETileStatus::OCCUPIED_BY_AI)
				{
					ReachableTiles.Add(CurrentTile);
				}
				else if (CurrentDistance > 0)
				{
					// Se è occupata e non è la tile di partenza non espandere oltre
					continue;
				}
			}
		}

		// Se ha raggiunto la distanza massima si ferma
		if (CurrentDistance >= MaxDistance)
			continue;

		TArray<FVector2D> Neighbors = GetNeighbors(CurrentPosition);

		for (FVector2D Neighbor : Neighbors)
		{
			if (!Visited.Contains(Neighbor) && IsValidPosition(Neighbor))
			{
				// Blocca la propagazione su tile con ostacoli o unità
				if (ObstacleMap.Contains(Neighbor))
					continue;

				if (TileMap.Contains(Neighbor))
				{
					ATile* NeighborTile = TileMap[Neighbor];
					if (NeighborTile)
					{
						ETileStatus Status = NeighborTile->GetTileStatus();
						if (Status == ETileStatus::OCCUPIED || Status == ETileStatus::OCCUPIED_BY_HUMAN || Status == ETileStatus::OCCUPIED_BY_AI)
							// Non esplora le tile occupate
							continue; 
					}
				}

				Visited.Add(Neighbor);
				Queue.Enqueue(TPair<FVector2D, int32>(Neighbor, CurrentDistance + 1));
			}
		}
	}

	return ReachableTiles;
}


// Metodo che restituisce i nemici attaccabili a partire da una posizione iniziale
TArray<ATile*> AGameField::GetAttackableEnemies(FVector2D StartPosition, int32 AttackRange, bool bIsHuman)
{
	TArray<ATile*> AttackableEnemies;
	// Set evita di visitare la stessa posizione più volte
	TSet<FVector2D> Visited; 
	TQueue<TPair<FVector2D, int32>> Queue;

	// Posizione di partenza
	Queue.Enqueue(TPair<FVector2D, int32>(StartPosition, 0));
	Visited.Add(StartPosition);

	while (!Queue.IsEmpty())
	{
		TPair<FVector2D, int32> Current;
		Queue.Dequeue(Current);

		FVector2D CurrentPosition = Current.Key;
		int32 CurrentDistance = Current.Value;

		// Solo se la distanza è inferiore o uguale al range di attacco Si considera la tile
		if (CurrentDistance <= AttackRange)
		{
			ATile* Tile = TileMap[CurrentPosition];
			if (Tile)
			{
				// Se l'unità è umana si cerca tile occupate da AI e viceversa
				if (bIsHuman)
				{
					// Aggiunge la tile se è occupata da un'AI
					if (Tile->GetTileStatus() == ETileStatus::OCCUPIED_BY_AI)
					{
						AttackableEnemies.Add(Tile);
					}
				}
				else
				{
					// Aggiunge la tile se è occupata da un umano
					if (Tile->GetTileStatus() == ETileStatus::OCCUPIED_BY_HUMAN)
					{
						AttackableEnemies.Add(Tile);
					}
				}
			}
		}

		// Non Si esplora oltre il range di attacco
		if (CurrentDistance >= AttackRange)
			continue;

		// Si ottiene i vicini escludendo gli ostacoli
		TArray<FVector2D> Neighbors = GetNeighbors(CurrentPosition);
		for (FVector2D Neighbor : Neighbors)
		{
			if (!Visited.Contains(Neighbor) && IsValidPosition(Neighbor))
			{
				Visited.Add(Neighbor);
				Queue.Enqueue(TPair<FVector2D, int32>(Neighbor, CurrentDistance + 1));
			}
		}
	}

	return AttackableEnemies;
}


// Metodo per trovare il percorso minimo
TArray<ATile*> AGameField::FindPath(FVector2D Start, FVector2D End)
{
	TArray<ATile*> Path;

	if (Start == End)
		return Path;

	// Mappa per ricostruire il percorso
	TMap<FVector2D, FVector2D> CameFrom;
	TSet<FVector2D> Visited;
	TQueue<FVector2D> Queue;

	// Se la destinazione è occupata, trova la cella adiacente più vicina nelle 4 direzioni cardinali
	ATile* EndTile = GetTileAtPosition(End);
	if (EndTile && EndTile->GetTileStatus() != ETileStatus::EMPTY)
	{
		TArray<FVector2D> CardinalDirections = { FVector2D(0,1), FVector2D(0,-1), FVector2D(1,0), FVector2D(-1,0) };
		FVector2D BestTarget = End;
		float MinDistance = FLT_MAX;

		for (FVector2D Direction : CardinalDirections)
		{
			FVector2D Candidate = End + Direction;
			ATile* CandidateTile = GetTileAtPosition(Candidate);
			if (CandidateTile && CandidateTile->GetTileStatus() == ETileStatus::EMPTY)
			{
				float Distance = (Start - Candidate).Size();
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
					BestTarget = Candidate;
				}
			}
		}

		End = BestTarget; // Imposta la nuova destinazione
	}

	// Avvia la BFS
	Queue.Enqueue(Start);
	Visited.Add(Start);
	CameFrom.Add(Start, Start);

	while (!Queue.IsEmpty())
	{
		FVector2D Current;
		Queue.Dequeue(Current);

		if (Current == End)
			break;

		TArray<FVector2D> Neighbors = GetNeighbors(Current);

		for (FVector2D Neighbor : Neighbors)
		{
			if (!Visited.Contains(Neighbor))
			{
				ATile* NeighborTile = GetTileAtPosition(Neighbor);
				if (NeighborTile && NeighborTile->GetTileStatus() == ETileStatus::EMPTY)
				{
					Queue.Enqueue(Neighbor);
					Visited.Add(Neighbor);
					CameFrom.Add(Neighbor, Current);
				}
			}
		}
	}

	// Se non è stato trovato un percorso, restituisce un array vuoto
	if (!CameFrom.Contains(End))
		return Path;

	// Ricostruisce il percorso
	FVector2D Current = End;
	while (Current != Start)
	{
		ATile* Tile = GetTileAtPosition(Current);
		if (Tile)
			Path.Insert(Tile, 0);
		Current = CameFrom[Current];
	}

	return Path;
}


/*
// Metodo per trovare il percorso minimo
TArray<ATile*> AGameField::FindPath(FVector2D Start, FVector2D End)
{
	TArray<ATile*> Path;

	if (Start == End)
		return Path;

	// MAppa per ricostruire il percorso
	TMap<FVector2D, FVector2D> CameFrom; 
	TSet<FVector2D> Visited;
	TQueue<FVector2D> Queue;

	Queue.Enqueue(Start);
	Visited.Add(Start);
	CameFrom.Add(Start, Start);

	while (!Queue.IsEmpty())
	{
		FVector2D Current;
		Queue.Dequeue(Current);

		if (Current == End)
			break;

		TArray<FVector2D> Neighbors = GetNeighbors(Current);

		for (FVector2D Neighbor : Neighbors)
		{
			if (!Visited.Contains(Neighbor))
			{
				ATile* NeighborTile = GetTileAtPosition(Neighbor);
				if (NeighborTile && NeighborTile->GetTileStatus() == ETileStatus::EMPTY) 
				{
					Queue.Enqueue(Neighbor);
					Visited.Add(Neighbor);
					CameFrom.Add(Neighbor, Current);
				}
			}
		}
	}

	// Se si arriva ad end si ricostruisce il path
	if (!CameFrom.Contains(End))
		// Nessun percorso trovato
		return Path; 

	FVector2D Current = End;
	while (Current != Start)
	{
		ATile* Tile = GetTileAtPosition(Current);
		if (Tile)
			Path.Insert(Tile, 0);
		Current = CameFrom[Current];
	}

	return Path;
}
*/


// NON IMPLEMENTATO ho provato a svilupparlo ma non ho avuto il tempo, l'ai è random
TArray<ATile*> AGameField::FindPathAStar(const FVector2D& Start, const FVector2D& Goal)
{
	struct FNode
	{
		FVector2D Position;
		float GCost;
		float HCost;
		float FCost() const { return GCost + HCost; }
		FVector2D Parent;

		FNode() {}
		FNode(FVector2D InPos, float InG, float InH, FVector2D InParent)
			: Position(InPos), GCost(InG), HCost(InH), Parent(InParent) {
		}
	};

	TArray<ATile*> Path;

	if (Start == Goal)
		return Path;

	TMap<FVector2D, FNode> AllNodes;
	TSet<FVector2D> ClosedSet;
	TArray<FNode> OpenSet;

	auto Heuristic = [](const FVector2D& A, const FVector2D& B)
		{
			return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y); 
		};

	OpenSet.Add(FNode(Start, 0.f, Heuristic(Start, Goal), Start));
	AllNodes.Add(Start, OpenSet[0]);

	while (OpenSet.Num() > 0)
	{

		int32 LowestIndex = 0;
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			if (OpenSet[i].FCost() < OpenSet[LowestIndex].FCost())
			{
				LowestIndex = i;
			}
		}

		FNode CurrentNode = OpenSet[LowestIndex];
		OpenSet.RemoveAt(LowestIndex);
		ClosedSet.Add(CurrentNode.Position);


		if (CurrentNode.Position == Goal)
		{
			FVector2D Pos = Goal;
			while (Pos != Start)
			{
				ATile* Tile = GetTileAtPosition(Pos);
				if (Tile)
					Path.Insert(Tile, 0);
				Pos = AllNodes[Pos].Parent;
			}
			return Path;
		}

		TArray<FVector2D> Neighbors = GetNeighbors(CurrentNode.Position);
		for (const FVector2D& NeighborPos : Neighbors)
		{
			if (ClosedSet.Contains(NeighborPos))
				continue;

			ATile* Tile = GetTileAtPosition(NeighborPos);
			if (!Tile || Tile->GetTileStatus() != ETileStatus::EMPTY)
				continue;

			float TentativeG = CurrentNode.GCost + 1.f;

			if (!AllNodes.Contains(NeighborPos) || TentativeG < AllNodes[NeighborPos].GCost)
			{
				FNode NeighborNode(NeighborPos, TentativeG, Heuristic(NeighborPos, Goal), CurrentNode.Position);
				AllNodes.Add(NeighborPos, NeighborNode);


				if (!OpenSet.ContainsByPredicate([&](const FNode& Node) { return Node.Position == NeighborPos; }))
				{
					OpenSet.Add(NeighborNode);
				}
			}
		}
	}


	return Path;
}

// Ritorna tutte le empty tile
TArray<ATile*> AGameField::GetAllEmptyTiles()
{
	TArray<ATile*> EmptyTiles;

	for (ATile* Tile : TileArray)
	{
		if (Tile && Tile->GetTileStatus() == ETileStatus::EMPTY)
		{
			EmptyTiles.Add(Tile);
		}
	}

	return EmptyTiles;
}


// Called every frame
//void AGameField::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}


