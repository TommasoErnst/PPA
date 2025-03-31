// Fill out your copyright notice in the Description page of Project Settings.


#include "PPA_HumanPlayer.h"
#include"PPA_GameModeBase.h"
#include "GameField.h"

// Sets default values
APPA_HumanPlayer::APPA_HumanPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//set the camera as RootComponent
	SetRootComponent(Camera);
	// get the game instance reference
	GameInstance = Cast<UPPA_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	// default init values
	PlayerNumber = -1;


}

int32 APPA_HumanPlayer::GetUnitType()
{
	return UnitType;
}

int32 APPA_HumanPlayer::GetUnitCounter()
{
	return UnitCounter;
}

// Called when the game starts or when spawned
void APPA_HumanPlayer::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UPPA_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
	GameMode = Cast<APPA_GameModeBase>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void APPA_HumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameMode && GameMode->GetHumanUnits().Num() <= 0 && IsSniperSpawned && IsBrawlerSpawned) {
		OnLose();
	}
	if (GameMode->GetAiUnits().Num() <= 0 && IsSniperSpawned && IsBrawlerSpawned) {
		OnWin();
	}
}

// Called to bind functionality to input
void APPA_HumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APPA_HumanPlayer::OnTurn()
{
	
		// Inizia il turno e resetta tutte le variabili necessarie
		GameInstance->SetTurnMessage(TEXT("Human Turn"));
		IsMyTurn = true;
		bSkipUniturn = false;
		SelectedUnit = nullptr;
		ReachableTiles.Empty(); // Resetta le tile raggiungibili
		AttackableEnemies.Empty(); // Resetta le tile nemiche attaccabili
		AttackCounter = 0; // Resetta il contatore degli attacchi
		OldSelectedUnit = nullptr; // Selezione unità precedente (se è una variabile utile)
		MinDmg = 0; // Resetta i danni minimi
		MaxDmg = 0; // Resetta i danni massimi
		if (IsSniperSpawned && IsBrawlerSpawned)
		{
			UnitHumanArray = GameMode->GetHumanUnits();

			for (ABaseUnit* Unit : UnitHumanArray)
			{
				if (Unit)
				{
					// Reset dei flag
					Unit->SetIsUnitMoved(false);
					Unit->SetHasUnitAttacked(false);
				}
			}
		}

		if (GameMode && GameMode->GetHumanUnits().Num() <= 0 && IsSniperSpawned && IsBrawlerSpawned) {
			OnLose();
		}

}

void APPA_HumanPlayer::OnWin()
{

	GameInstance->SetTurnMessage(TEXT("Human Wins!"));
	IsMyTurn = false;
	GameMode->IsGameOver = true;
}

void APPA_HumanPlayer::OnLose()
{
	GameInstance->SetTurnMessage(TEXT("Ai Wins"));
	IsMyTurn = false;
	GameMode->IsGameOver = true;
}

bool APPA_HumanPlayer::GetbSkipUniturn()
{
	return bSkipUniturn;
}

void APPA_HumanPlayer::AsyncSetUintHasAttacked()
{
	SelectedUnit->SetHasUnitAttacked(true);

	for (ATile* Tile3 : AttackableEnemies)
		if (Tile3) Tile3->SetHighlightedEnemy(false);

	if (IsSniperSpawned && IsBrawlerSpawned) {

		bool bAllUnitsAttacked = true;

		for (ABaseUnit* Unit : UnitHumanArray)
		{
			if (Unit && !Unit->GetHasUnitAttacked())
			{
				bAllUnitsAttacked = false;
				break;
			}
		}

		if (bAllUnitsAttacked)
		{
			IsMyTurn = false;
			GameMode->TurnNextPlayer();
		}
	}
}


// Metodo per spawnare le unit
void APPA_HumanPlayer::HandleSpawn(ATile* Tile)
{
	// Se UnitType è -1, significa che nessuna unità è selezionata
	if (UnitType == -1) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Seleziona un'unità prima di spawnarla!"));
		return; // Non spawna nulla se l'unità non è selezionata dai bottoni in gioco
	}

	if (Tile->GetTileStatus() == ETileStatus::EMPTY) {
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Spawning unit")));

		Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED_BY_HUMAN);
		FVector SpawnLocation = Tile->GetActorLocation() + FVector(0, 0, 5);

		if (UnitType == 0) { // Sniper
			// Logica per spawnare Sniper
			GameMode->SetTileUnit(PlayerNumber, SpawnLocation, 0, Tile); // 0 per Sniper
		}
		else { // Brawler
			// Logica per spawnare Brawler
			GameMode->SetTileUnit(PlayerNumber, SpawnLocation, 1, Tile); // 1 per Brawler
		}

		IsMyTurn = false;
		if (UnitType == 0) {
			IsSniperSpawned = true;
			GameInstance->bIsSniperSpawned = true;
		}
		else {
			IsBrawlerSpawned = true;
			GameInstance->bIsBrawlerSpawned = true;
		}

		// Resetta UnitType per evitare che l'unità venga spawnata più volte
		UnitType = -1; // Imposta a -1 per segnalare che il tipo di unità è stato usato
		UnitCounter ++;

		GameMode->TurnNextPlayer();
		return;
	}
}

// Logica per la selezione della unit
void APPA_HumanPlayer::HandleUnitSelection(ABaseUnit* Unit)
{

	// Qui seleziono una unit e viene mostrato il range di movimento e i nemici attaccabili
	if (HeldBy == 1) {

		// Ottiene la posizione iniziale del Brawler
		if (Unit == SelectedUnit && Unit->GetIsUnitMoved()) {
			return;
		}
		if (Unit->GetIsUnitMoved()) {
			bSkipUniturn = true;
		}

		SelectedUnit = Unit;

		FVector2D StartPos = SelectedUnit->GetGridPosition();
		int32 MaxDistance = SelectedUnit->MaxMovement;
		int32 AttackRange = SelectedUnit->AttackRange;
		MinDmg = SelectedUnit->MinDamage;
		MaxDmg = SelectedUnit->MaxDamage;

		// Ottiene le tile raggiungibili
		ReachableTiles = GameField->GetReachableTiles(StartPos, MaxDistance);
		AttackableEnemies = GameField->GetAttackableEnemies(StartPos, AttackRange, true);

		// Evidenzia le tile raggiungibili
		GameMode->SetHighlightedTileStatus(Unit, ReachableTiles, AttackableEnemies);

	}


	// Qui ho già selezionato la unit che attaccherà il nemico e di conseguenza si sviluppa il metodo per il danno
	if (SelectedUnit != nullptr && HeldBy == 0) {

		ATile* EnemyPosition = Unit->GetCurrentTile();
		if (AttackableEnemies.Num() > 0 && AttackableEnemies.Contains(EnemyPosition) && !SelectedUnit->GetHasUnitAttacked())
		{
			int32 Damage = FMath::RandRange(MinDmg, MaxDmg);

			// Riduce la salute dell'unità nemica
			Unit->Health = Unit->Health - Damage;

			// Applicazione danno da contrattacco (per lo Sniper)
			if (SelectedUnit->IsA(ASniper::StaticClass()))
			{
				FVector2D SniperPos = SelectedUnit->GetCurrentTile()->GetGridPosition();
				FVector2D EnemyPos = Unit->GetCurrentTile()->GetGridPosition();

				// Verifica se l'unità attaccata è uno Sniper o un Brawler a distanza 1
				if (Unit->IsA(ASniper::StaticClass()) ||
					(Unit->IsA(ABrawler::StaticClass()) &&
						FMath::Abs(SniperPos.X - EnemyPos.X) <= 1 &&
						FMath::Abs(SniperPos.Y - EnemyPos.Y) <= 1))
				{
					int32 CounterDamage = FMath::RandRange(1, 3);  // Danno da contrattacco
					SelectedUnit->Health -= CounterDamage;  // Applica il danno allo Sniper
					GameInstance->HumanSniperHealth = SelectedUnit->Health;

					// Log del danno da contrattacco 
					GameMode->LogActionMessage(Unit, Unit->GetCurrentTile(), SelectedUnit->GetCurrentTile(), CounterDamage);
				}
			}

			// Logica per aggiornare la salute dell'unità nemica (per AiBrawler e AiSniper)
			if (Unit->IsA(ABrawler::StaticClass()))
			{
				GameInstance->AiBrawlerHealth = Unit->Health;
			}
			else if (Unit->IsA(ASniper::StaticClass()))
			{
				GameInstance->AiSniperHealth = Unit->Health;
			}

			
			if (Unit->Health <= 0) {
				GameMode->RemoveUnit(Unit);
			}

			// Stampo il messaggio sulla scrollbox
			GameMode->LogActionMessage(SelectedUnit, SelectedUnit->GetCurrentTile(), EnemyPosition, Damage);

			// Marca l'unità come aver effettuato l'attacco
			SelectedUnit->SetHasUnitAttacked(true);

			// Disabilita l'evidenziazione delle tiles raggiungibili e nemiche
			for (ATile* Tile2 : ReachableTiles)
				if (Tile2) Tile2->SetHighlighted(false);
			for (ATile* Tile3 : AttackableEnemies)
				if (Tile3) Tile3->SetHighlightedEnemy(false);

			// Verifica se la partita è vinta (tutte le unità nemiche sono morte)
			if (GameMode->GetAiUnits().Num() <= 0) {
				OnWin();
			}

			// Verifica se tutte le unità hanno attaccato
			bool bAllUnitsAttacked = true;
			for (ABaseUnit* Unit : UnitHumanArray)
			{
				if (Unit && !Unit->GetHasUnitAttacked())
				{
					bAllUnitsAttacked = false;
					break;
				}
			}

			//Se tutte le unità hanno attaccato passa al prossimo player
			if (bAllUnitsAttacked)
			{
				IsMyTurn = false;
				GameMode->TurnNextPlayer();
			}
		}
		else {
			return;
		}
	}
}



void APPA_HumanPlayer::OnClick()
{
	if (IsMyTurn) {
		
		FHitResult Hit;
		GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

		if (Hit.bBlockingHit) 
		{
			bSkipUniturn = false;
			AActor* HitActor = Cast<AActor>(Hit.GetActor());
			if (HitActor)
			{
				// Spawno le unit dopo averle spawnate inizierà la logica di gioco
				if (!IsSniperSpawned || !IsBrawlerSpawned)
				{
					if (ATile* Tile = Cast<ATile>(HitActor))
					{
						HandleSpawn(Tile);
					}
				}

				// Units spawnate adesso procedo con la logica di selezione della unit
				if (IsSniperSpawned && IsBrawlerSpawned)
				{
					if (GameMode->GetAiUnits().Num() <= 0) {
						OnWin();
					}
					if (ABaseUnit* Unit = Cast<ABaseUnit>(HitActor))
					{
						HeldBy = Unit->GetHeldByPlayer();

						if (UnitHumanArray.Contains(Unit) && Unit->GetHasUnitAttacked())
						{
							return;
						}
						else
						{
							// Unità selezionata
							HandleUnitSelection(Unit);
						}
					}
					 

					// Entro qui sotto nel momento in cui le unità sono state spawnate e ho cliccato una delle mie unità
					// Questa sotto rappresenta la logica di movimento
					if (SelectedUnit != nullptr)
					{

						if (ATile* Tile = Cast<ATile>(HitActor))
						{
							if (ReachableTiles.Contains(Tile)) {
								
								// Faccio muovere la mia pedina
								GameMode->LogActionMessage(SelectedUnit, SelectedUnit->GetCurrentTile(), Tile);
								MoveUnitToTile(Tile);
								 
								bSkipUniturn = true;


								int32 AttackRange = SelectedUnit->AttackRange;
								FVector2D UnitPos = SelectedUnit->GetGridPosition();
								AttackableEnemies = GameField->GetAttackableEnemies(UnitPos, AttackRange, true);

								// Una volta mosso se non ho nessun nemico attaccabile faccio in modo che il turno di quella unit finisca
								if (AttackableEnemies.Num() <= 0) {
									SelectedUnit->SetHasUnitAttacked(true);
									bool bAllHumansAttacked = true;
									bSkipUniturn = false;

									// Controllo se le azioni delle mie unit sono esaurite
									for (ABaseUnit* Unit : UnitHumanArray)
									{
										if (!Unit->GetHasUnitAttacked())
										{
											bAllHumansAttacked = false;
											break;
										}
									}

									if (bAllHumansAttacked)
									{
										IsMyTurn = false;
										GameMode->TurnNextPlayer();
									}
								}

								//Altrimmenti continuo la logica verso l'attacco dove selezionero la prossima unità cliccando quella AI
								else{
									for (ATile* Tile3 : AttackableEnemies)
										if (Tile3) Tile3->SetHighlightedEnemy(true);
								}
							}

							// Se clicco al di fuori del range di movimento deseleziono la mia unit
							else {
								for (ATile* Tile3 : ReachableTiles)
									if (Tile3) Tile3->SetHighlighted(false);
								for (ATile* Tile4 : AttackableEnemies)
									if (Tile4) Tile4->SetHighlightedEnemy(false);
								SelectedUnit = nullptr;
							}
						}
					}
				}
			}
		}
	}
}




// logica per muovere la unit step by step
void APPA_HumanPlayer::MoveUnitStepByStep()
{
	
	if (SelectedUnit == nullptr)
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		return;
	}
	if (CurrentStepIndex >= SelectedPath.Num())
	{
		// Movimento finito
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		return;
		IsMyTurn = true;
	}

	ATile* NextTile = SelectedPath[CurrentStepIndex];
	if (NextTile)
	{
		// Sposta l'unità alla posizione della prossima tile
		FVector NewLocation = NextTile->GetActorLocation() + FVector(0, 0, 5); // Leggero offset Z

		SelectedUnit->SetActorLocation(NewLocation);

		// Aggiorna posizione interna (griglia)
		SelectedUnit->SetCurrentTile(NextTile);

		CurrentStepIndex++;
	}
	
}

// Muove la tile alla posizzione secono il path minimo
void APPA_HumanPlayer::MoveUnitToTile(ATile* Tile)
{
	if (SelectedUnit == nullptr)
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		return;
	}


	if (!SelectedUnit) return;


	FVector2D UnitPos = SelectedUnit->GetGridPosition();
	int32 MaxDistance = SelectedUnit->MaxMovement;
	int32 AttackRange = SelectedUnit->AttackRange;
	ATile* StartingTile = SelectedUnit->GetCurrentTile();
	StartingTile->SetTileStatus(-1, ETileStatus::EMPTY);

	// Rimuove highlight
	for (ATile* Tile2 : ReachableTiles)
		if (Tile2) Tile2->SetHighlighted(false);

	for (ATile* Tile3 : AttackableEnemies)
		if (Tile3) Tile3->SetHighlightedEnemy(false);

	// Se la tile cliccata è raggiungibile e libera
	if (ReachableTiles.Contains(Tile) && Tile->GetTileStatus() == ETileStatus::EMPTY)
	{
		//  Libera la vecchia tile
		ATile* OldTile = GameField->GetTileAtPosition(UnitPos);
		if (OldTile)
			OldTile->SetTileStatus(-1, ETileStatus::EMPTY);

		//  Calcola il percorso
		SelectedPath = GameField->FindPath(UnitPos, Tile->GetGridPosition());

		//  Avvia movimento passo a passo
		CurrentStepIndex = 0;
		GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &APPA_HumanPlayer::MoveUnitStepByStep, 0.5f, true);

		
		//  Segna la nuova tile come occupata
		Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED_BY_HUMAN);
		SelectedUnit->SetCurrentTile(Tile);

		// Dopo il movimento (verifica se è già completo)
		if (CurrentStepIndex >= SelectedPath.Num())
		{
			GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		}
	}
	SelectedUnit->SetIsUnitMoved(true);

}




