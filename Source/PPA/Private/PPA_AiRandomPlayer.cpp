// Fill out your copyright notice in the Description page of Project Settings.


#include "PPA_AiRandomPlayer.h"
#include "PPA_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Brawler.h"

// Sets default values
APPA_AiRandomPlayer::APPA_AiRandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APPA_AiRandomPlayer::OnTurn()
{
	GameInstance->SetTurnMessage(TEXT("AI turn"));

	HumanUnitsArray = GameMode->GetHumanUnits();
	FTimerHandle TimerHandle;
	CurrentUnitIndex = 0;
	Units = GameMode->GetAiUnits();
	for (ABaseUnit* Unit : Units)
	{
		if (Unit)
		{
			Unit->SetHasUnitAttacked(false);
			Unit->SetIsUnitMoved(false);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APPA_AiRandomPlayer::StartAiTurnLogic, 3.f, false);
}

// Called every frame
void APPA_AiRandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APPA_AiRandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void APPA_AiRandomPlayer::OnWin()
{
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameMode->TurnNextPlayer();
}

void APPA_AiRandomPlayer::OnLose()
{
	GameInstance->SetTurnMessage(TEXT("Human Wins!"));
}


// Called when the game starts or when spawned
void APPA_AiRandomPlayer::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPPA_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
	GameMode = Cast<APPA_GameModeBase>(GetWorld()->GetAuthGameMode());
	
}

// Metodo per spawnare le unit
void APPA_AiRandomPlayer::SpawnUnit()
{
	TArray<ATile*> EmptyTiles = GameField->GetAllEmptyTiles();

	if (EmptyTiles.Num() == 0) return;

	ATile* RandomTile = EmptyTiles[FMath::RandRange(0, EmptyTiles.Num() - 1)];
	FVector SpawnLocation = RandomTile->GetActorLocation() + FVector(0, 0, 5);

	if (UnitType == -1) {
		UnitType = FMath::RandRange(2, 3);
	}

	RandomTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED_BY_AI);

	// UnitType serve per identificare quale tipo di unit spawnare se rossa/blu e se human/ai
	ABaseUnit* SpawnedUnit = GameMode->SetTileUnit(PlayerNumber, SpawnLocation, UnitType, RandomTile);
	if (SpawnedUnit)
	{
		//spawno sniper ai
		if (UnitType == 2) {
			IsSniperSpawned = true;
			UnitType = 3;
		}
		//spawno brawler ai
		else {
			IsBrawlerSpawned = true;
			UnitType = 2;
		}
	}
	
}


// Logica principale per l'ai
void APPA_AiRandomPlayer::StartAiTurnLogic()
{
	// Si spawnano le unità
	if (!IsSniperSpawned || !IsBrawlerSpawned)
	{
		SpawnUnit();
		GameMode->TurnNextPlayer();
		return;
	}

	if (Units.Num() == 0)
	{
		GameMode->TurnNextPlayer();
		return;
	}

	CurrentUnitIndex = 0;
	// Metodo che continua la logica ai
	HandleNextUnit();
}



// Logica ai principle
void APPA_AiRandomPlayer::HandleNextUnit()
{
	if (GameMode->GetHumanUnits().Num() <= 0) {
		return;
	}
	 // Si illuminano le reachable tiles
	for (ATile* tile : ReachableTiles)
	{
		if (tile) tile->SetHighlighted(false);
	}
	// Se finisco le mosse passo allo human pplayer
	if (CurrentUnitIndex >= Units.Num())
	{
		GameMode->TurnNextPlayer();
		return;
	}

	SelectedUnit = Units[CurrentUnitIndex];

	CurrentUnitIndex++;

	if (!SelectedUnit || SelectedUnit->GetHasUnitAttacked())
	{
		HandleNextUnit();
		return;
	}

	// si muove verso una random tile e prov ad attaccare
	
	MoveTowardsClosestEnemy();

	// Messaggio per la scrollbar se l'unità si è mossa

	if(SelectedUnit->GetIsUnitMoved())
	GameMode->LogActionMessage(SelectedUnit, SelectedUnit->GetCurrentTile(), FinalPos);
}





// Algoritmo per muoversi verso il nemico più vicino
void APPA_AiRandomPlayer::MoveTowardsClosestEnemy()
{
	if (!SelectedUnit || HumanUnitsArray.Num() == 0)
	{
		return;
	}

	FVector2D StartPos = SelectedUnit->GetGridPosition();
	int32 MaxDistance = SelectedUnit->MaxMovement;
	ReachableTiles = GameField->GetReachableTiles(StartPos, MaxDistance);
	ATile* StartTile = GameField->GetTileAtPosition(StartPos);
	

	for (ATile* Tile : ReachableTiles)
	{
		if (Tile) Tile->SetHighlighted(true);
	}

	// Trova il nemico più vicino
	ABaseUnit* ClosestEnemy = nullptr;
	float MinDistance = FLT_MAX;

	for (ABaseUnit* EnemyUnit : HumanUnitsArray)
	{
		if (EnemyUnit)
		{
			float Distance = (StartPos - EnemyUnit->GetGridPosition()).Size();
	

			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestEnemy = EnemyUnit;
			}
		}
	}

	if (!ClosestEnemy)
	{
	
		return;
	}

	FVector2D TargetPos = ClosestEnemy->GetGridPosition();
	FinalPos = ClosestEnemy->GetCurrentTile();
	
	// Se già in range attacca senza muoversi
	if (MinDistance <= SelectedUnit->AttackRange)
	{
	
		AttackRandomEnemy();

		SelectedUnit->SetHasUnitAttacked(true);

		// Dopo il movimento (e possibile attacco), passa alla prossima unità
		FTimerHandle NextUnitTimer;
		GetWorld()->GetTimerManager().SetTimer(NextUnitTimer, this, &APPA_AiRandomPlayer::HandleNextUnit, 2.0f, false);
		return;
	}

	// Se non è possibile attaccare parte la logica per muoversi
	else{

		SelectedPath = GameField->FindPath(StartPos, TargetPos);


		if (SelectedPath.Num() > MaxDistance)
		{
			SelectedPath.SetNum(MaxDistance);
		}

		if (SelectedPath.Num() == 0)
		{
			return;
		}


		StartTile->SetTileStatus(-1, ETileStatus::EMPTY);
		CurrentStepIndex = 0;
		SelectedUnit->SetIsUnitMoved(true);
		GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &APPA_AiRandomPlayer::MoveAiUnitStepByStep, 0.8f, true);
		
	}

}






// Muove l'unit per ogni casella attraverso il path minimo
void APPA_AiRandomPlayer::MoveAiUnitStepByStep()
{
	if (!SelectedUnit || CurrentStepIndex >= SelectedPath.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);

		if (SelectedPath.Num() > 0)
		{
			ATile* FinalTile = SelectedPath.Last();
			FinalTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED_BY_AI);
			SelectedUnit->SetCurrentTile(FinalTile);


			// Tenta attacco dopo il movimento
			FVector2D NewPos = FinalTile->GetGridPosition();
			int32 AttackRange = SelectedUnit->AttackRange;

			TArray<ATile*> AttackableEnemies = GameField->GetAttackableEnemies(NewPos, AttackRange, false);
			if (AttackableEnemies.Num() > 0)
			{
				AttackRandomEnemy();
			}
		}

		// Unità ha già agito
		SelectedUnit->SetHasUnitAttacked(true);

		// Dopo il movimento (e possibile attacco), passa alla prossima unità
		FTimerHandle NextUnitTimer;
		GetWorld()->GetTimerManager().SetTimer(NextUnitTimer, this, &APPA_AiRandomPlayer::HandleNextUnit, 2.0f, false);
		return;
	}

	ATile* StepTile = SelectedPath[CurrentStepIndex];
	if (StepTile)
	{
		SelectedUnit->SetActorLocation(StepTile->GetActorLocation() + FVector(0, 0, 5));
		SelectedUnit->SetCurrentTile(StepTile);
	}

	CurrentStepIndex++;
}





// Attacca un nemico random tra quelli attaccabili
void APPA_AiRandomPlayer::AttackRandomEnemy()
{

	if (!SelectedUnit)
	{
		return;
	}

	// Trova le unità nemiche che sono nel raggio di attacco
	FVector2D StartPos = SelectedUnit->GetGridPosition();
	int32 AttackRange = SelectedUnit->AttackRange;

	// Ottieni tutte le unità nemiche che sono nel raggio di attacco
	TArray<ABaseUnit*> EnemiesInRange;
	for (ABaseUnit* EnemyUnit : HumanUnitsArray)
	{
		// Verifica se l'unità nemica è nel raggio di attacco
		if (EnemyUnit)
		{
			FVector2D EnemyPos = EnemyUnit->GetGridPosition();
			if ((StartPos - EnemyPos).Size() <= AttackRange)
			{
				EnemiesInRange.Add(EnemyUnit);
			}
		}
	}

	// Se ci sono nemici nel raggio di attacco, attacca uno di loro casualmente
	if (EnemiesInRange.Num() > 0)
	{
		// Scegli un nemico casuale
		ABaseUnit* TargetEnemy = EnemiesInRange[FMath::RandRange(0, EnemiesInRange.Num() - 1)];

		// Attacca il nemico
		if (TargetEnemy)
		{
			// Calcola danno
			int32 Damage = FMath::RandRange(SelectedUnit->MinDamage, SelectedUnit->MaxDamage);

			// Riduce i punti vita del nemico
			TargetEnemy->Health = TargetEnemy->Health - Damage;

			GameMode->LogActionMessage(SelectedUnit, TargetEnemy->GetCurrentTile(), SelectedUnit->GetCurrentTile(), Damage);

			// Se l'unità attaccata è uno Sniper o un Brawler a distanza 1, contrattacca
			if (TargetEnemy->IsA(ASniper::StaticClass()) ||
				(TargetEnemy->IsA(ABrawler::StaticClass()) &&
					FMath::Abs(TargetEnemy->GetGridPosition().X - SelectedUnit->GetGridPosition().X) <= 1 &&
					FMath::Abs(TargetEnemy->GetGridPosition().Y - SelectedUnit->GetGridPosition().Y) <= 1))
			{
				// Danno da contrattacco per lo Sniper
				int32 CounterDamage = FMath::RandRange(1, 3);  // Danno da contrattacco
				SelectedUnit->Health -= CounterDamage;  // Applica il danno allo Sniper
				//GameInstance->AiSniperHealth = GameInstance->AiSniperHealth - CounterDamage;

				//Se l'unit muore dal contrattacco si rimuove
				if (SelectedUnit->Health <= 0) {
					GameMode->RemoveUnit(SelectedUnit);
				}

				// Log del danno da contrattacco
				GameMode->LogActionMessage(TargetEnemy, SelectedUnit->GetCurrentTile(), TargetEnemy->GetCurrentTile(), CounterDamage);
			}

			// Logica per aggiornare la salute dell'unità nemica (per HumanBrawler e HumanSniper)
			if (TargetEnemy->IsA(ABrawler::StaticClass()))
			{
				GameInstance->HumanBrawlerHealth = TargetEnemy->Health;
			}
			else if (TargetEnemy->IsA(ASniper::StaticClass()))
			{
				GameInstance->HumanSniperHealth = TargetEnemy->Health;
			}
			

			// Se l'unità nemica muore si rimuove
			if (TargetEnemy->Health <= 0) {
				GameMode->RemoveUnit(TargetEnemy);
			}

			if (GameMode->GetHumanUnits().Num() <= 0) {
				OnWin();
			}

		}
	}
}




//Metodo per muoversi ad una random tile NON PRESENTE NELLA LOGICA, UTILIZZATTO PER LO SVILUPPO INIZIALE
void APPA_AiRandomPlayer::MoveToRandomTile()
{
	if (!SelectedUnit) return;

	FVector2D StartPos = SelectedUnit->GetGridPosition();
	int32 MaxDistance = SelectedUnit->MaxMovement;
	ATile* StartTile = GameField->GetTileAtPosition(StartPos);
	StartTile->SetTileStatus(-1, ETileStatus::EMPTY);

	ReachableTiles = GameField->GetReachableTiles(StartPos, MaxDistance);

	if (ReachableTiles.Num() == 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("NESSUNA TILE RAGGIUNGIBILE")));
	}

	for (ATile* tile : ReachableTiles)
	{
		if (tile) tile->SetHighlighted(true);
	}

	ATile* TargetTile = ReachableTiles[FMath::RandRange(0, ReachableTiles.Num() - 1)];
	FinalPos = TargetTile;
	SelectedPath = GameField->FindPath(StartPos, TargetTile->GetGridPosition());

	if (SelectedPath.Num() == 0) return;

	CurrentStepIndex = 0;
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &APPA_AiRandomPlayer::MoveAiUnitStepByStep, 0.8f, true);
}
