// Fill out your copyright notice in the Description page of Project Settings.


#include "PPA_GameModeBase.h"

const TArray<ABaseUnit*>& APPA_GameModeBase::GetHumanUnits() const
{
	return HumanUnits;
}

const TArray<ABaseUnit*>& APPA_GameModeBase::GetAiUnits() const
{
	return AiUnits;
}

void APPA_GameModeBase::LogActionMessage(ABaseUnit* Unit, ATile* FromTile, ATile* ToTile, int32 Damage)
{
	if (!Unit || !FromTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("LogActionMessage: Unit o FromTile nulli"));
		return;
	}

	FString PlayerID = Unit->GetHeldByPlayer() == 1 ? TEXT("HP") : TEXT("AI");
	FString UnitID = Unit->IsA(ASniper::StaticClass()) ? TEXT("S") : TEXT("B");

	FString FromTileID = FromTile->GetTileID(); 
	FString ToTileID = ToTile ? ToTile->GetTileID() : "";

	FString Message;

	if (ToTile && Damage < 0)
	{
		// Movimento
		Message = FString::Printf(TEXT("%s: %s  %s ->  %s"), *PlayerID, *UnitID, *FromTileID, *ToTileID);
	}
	else if (Damage >= 0 && ToTile)
	{
		// Attacco
		Message = FString::Printf(TEXT("%s: %s  %s  %d"), *PlayerID, *UnitID, *ToTileID, Damage);
	}
	else
	{
		// Parametri insufficienti
		UE_LOG(LogTemp, Warning, TEXT("LogActionMessage: Parametri non validi"));
		return;
	}

	// Aggiunge al GameInstance
	if (UPPA_GameInstance* GI = Cast<UPPA_GameInstance>(GetGameInstance()))
	{
		GI->AddActionTurnMessage(Message);
		
	}
}

APPA_GameModeBase::APPA_GameModeBase()
{
	//PlayerControllerClass = ATTT_PlayerController::StaticClass();
	DefaultPawnClass = APPA_HumanPlayer::StaticClass();
	FieldSize = 25;
}

void APPA_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPPA_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	IsGameOver = false;

	MoveCounter = 0;

	//ATTT_HumanPlayer* HumanPlayer = *TActorIterator<ATTT_HumanPlayer>(GetWorld());
	APPA_HumanPlayer* HumanPlayer = GetWorld()->GetFirstPlayerController()->GetPawn<APPA_HumanPlayer>();

	if (!IsValid(HumanPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("No player pawn of type '%s' was found."), *APPA_HumanPlayer::StaticClass()->GetName());
		return;
	}

	if (GameFieldClass != nullptr)
	{
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->Size = FieldSize;
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = ((GField->TileSize * FieldSize) + ((FieldSize - 1) * GField->TileSize * GField->CellPadding)) * 0.5f;
	float Zposition = 3500.0f;
	FVector CameraPos(CameraPosX, CameraPosX, Zposition);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());


	Players.Add(HumanPlayer);
	// Random Player
	auto* AI = GetWorld()->SpawnActor<APPA_AiRandomPlayer>(FVector(), FRotator());
	Players.Add(AI);

	this->ChoosePlayerAndStartGame();

}

void APPA_GameModeBase::ChoosePlayerAndStartGame()
{
	 CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

	for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
	{
		Players[IndexI]->PlayerNumber = IndexI;
	}
	MoveCounter += 1;

	Players[CurrentPlayer]->OnTurn(); 
}


int32 APPA_GameModeBase::GetNextPlayer(int32 Player)
{
	if (!IsGameOver) {
		Player++;
		if (!Players.IsValidIndex(Player))
		{
			Player = 0;
		}
		return Player;
	}
	return 0;
}

void APPA_GameModeBase::TurnNextPlayer()
{
	MoveCounter += 1;
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	Players[CurrentPlayer]->OnTurn();
	FString Msg = FString::Printf(TEXT("Turno del Giocatore %d"), CurrentPlayer);
}


// Logica per lo spawn effettivo delle unit  0 e 2 sniper human e ai mentre 1 e 3 brawler human e ai
ABaseUnit* APPA_GameModeBase::SetTileUnit(const int32 PlayerNumber, const FVector& SpawnPosition, const int UnitType, ATile* Tile)
{
	FVector Location = GField->GetActorLocation() + SpawnPosition;
	FRotator Rotation = FRotator::ZeroRotator;

	if (UnitType == 0 || UnitType == 2) {
		ASniper* SpawnedSniper = nullptr;

		if (UnitType == 0) {
			SpawnedSniper = GetWorld()->SpawnActor<ASniper>(SniperHumanClass, Location, Rotation);
			SpawnedSniper->SetHeldByPlayer(1);
			HumanUnits.Add(SpawnedSniper); // Aggiunto qui

			GameInstance->HumanSniperHealth=SpawnedSniper->Health;

		}
		else {
			SpawnedSniper = GetWorld()->SpawnActor<ASniper>(SniperAiClass, Location, Rotation);
			SpawnedSniper->SetHeldByPlayer(0);
			AiUnits.Add(SpawnedSniper); // Aggiunto qui

			GameInstance->AiSniperHealth = SpawnedSniper->Health;

		}

		SpawnedSniper->SetCurrentTile(Tile);
		//FVector2D GridPos = SpawnedSniper->GetGridPosition();
		//FString Msg = FString::Printf(TEXT("POZISIONE SNIPER: X=%.0f, Y=%.0f"), GridPos.X, GridPos.Y);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
		return SpawnedSniper;
	}

	if (UnitType == 1 || UnitType == 3) {
		ABrawler* SpawnedBrawler = nullptr;

		if (UnitType == 1) {
			SpawnedBrawler = GetWorld()->SpawnActor<ABrawler>(BrawlerHumanClass, Location, Rotation);
			SpawnedBrawler->SetHeldByPlayer(1);
			HumanUnits.Add(SpawnedBrawler); // Aggiunto qui

			GameInstance->HumanBrawlerHealth = SpawnedBrawler->Health;

		}
		else {
			SpawnedBrawler = GetWorld()->SpawnActor<ABrawler>(BrawlerAiClass, Location, Rotation);
			SpawnedBrawler->SetHeldByPlayer(0);
			AiUnits.Add(SpawnedBrawler); // Aggiunto qui

			GameInstance->AiBrawlerHealth = SpawnedBrawler->Health;

		}

		SpawnedBrawler->SetCurrentTile(Tile);
		//FVector2D GridPos = SpawnedBrawler->GetGridPosition();
		//FString Msg = FString::Printf(TEXT("POZISIONE BRAWLER: X=%.0f, Y=%.0f"), GridPos.X, GridPos.Y);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
		return SpawnedBrawler;
	}

	return nullptr;
}


// Metodo per illuminare le tile raggiungibili
void APPA_GameModeBase::SetHighlightedTileStatus(ABaseUnit* SelectedUnit, const TArray<ATile*>& ReachableTiles, const TArray<ATile*>& AttackableEnemies)
{
	// Se è stata selezionata una nuova unità gli highlight si spengono
	if (CurrentlySelectedUnit != nullptr && CurrentlySelectedUnit != SelectedUnit)
	{
		for (ATile* Tile : OldReachableTiles)
			if (Tile) Tile->SetHighlighted(false);

		for (ATile* Tile : OldAttackableEnemies)
			if (Tile) Tile->SetHighlightedEnemy(false);

		OldReachableTiles.Empty();
		OldAttackableEnemies.Empty();

		if (bIsHighlighted) bIsHighlighted = !bIsHighlighted;
	}

	// Set nuovi highlight
	if (!bIsHighlighted) {
		if (!SelectedUnit->GetIsUnitMoved())
		{
			for (ATile* Tile : ReachableTiles)
				if (Tile) Tile->SetHighlighted(true);
		}
		for (ATile* Tile : AttackableEnemies)
			if (Tile) Tile->SetHighlightedEnemy(true);
		bIsHighlighted = true;
	}
	else {
		for (ATile* Tile : ReachableTiles)
			if (Tile) Tile->SetHighlighted(false);

		for (ATile* Tile : AttackableEnemies)
			if (Tile) Tile->SetHighlightedEnemy(false);
		bIsHighlighted = false;
	}

	// Aggiorna lo stato
	OldReachableTiles = ReachableTiles;
	OldAttackableEnemies = AttackableEnemies;
	CurrentlySelectedUnit = SelectedUnit;
}


//Metodo per rimuovere la unit
void APPA_GameModeBase::RemoveUnit(ABaseUnit* UnitToRemove)
{
	if (!UnitToRemove) return;

	ATile* Tile = UnitToRemove->GetCurrentTile();
	if (Tile)
	{
		Tile->SetTileStatus(-1 ,ETileStatus::EMPTY);
	}

	// Rimuove dall’array corretto
	if (UnitToRemove->GetHeldByPlayer() == 1)
	{
		HumanUnits.Remove(UnitToRemove);
	}
	else
	{
		AiUnits.Remove(UnitToRemove);
	}


	UnitToRemove->Destroy();
}
