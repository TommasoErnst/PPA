// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Brawler.h"
#include "Sniper.h"
#include "PPA_GameInstance.generated.h"


/**
 * 
 */
UCLASS()
class PPA_API UPPA_GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:


	// score value for human player
	UPROPERTY(EditAnywhere)
	int32 ScoreHumanPlayer = 0;

	// score value for AI player
	UPROPERTY(EditAnywhere)
	int32 ScoreAiPlayer = 0;

	UPROPERTY(EditAnywhere)
	bool bIsBrawlerSpawned = false;

	UPROPERTY(EditAnywhere)
	bool bIsSniperSpawned = false;

	UPROPERTY(EditAnywhere)
	int32 HumanSniperHealth = 0;

	UPROPERTY(EditAnywhere)
	int32 HumanBrawlerHealth = 0;

	UPROPERTY(EditAnywhere)
	int32 AiBrawlerHealth = 0;

	UPROPERTY(EditAnywhere)
	int32 AiSniperHealth = 0;

	// message to show every turn
	UPROPERTY(EditAnywhere)
	FString CurrentTurnMessage = "Current Player";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> ActionTurnMessages;

	// get the current turn message
	UFUNCTION(BlueprintCallable)
	bool GetIsBrawlerSpawned();

	UFUNCTION(BlueprintCallable)
	bool GetIsSniperSpawned();


	UFUNCTION(BlueprintCallable)
	int32 GetHumanSniperHealth();

	UFUNCTION(BlueprintCallable)
	int32 GetAiSniperHealth();

	UFUNCTION(BlueprintCallable)
	int32 GetAiBrawlerHealth();

	UFUNCTION(BlueprintCallable)
	int32 GetHumanBrawlerHealth();


	UFUNCTION(BlueprintCallable)
	FString GetTurnMessage();

	// set the turn message
	UFUNCTION(BlueprintCallable)
	void SetTurnMessage(FString Message);

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetActionTurnMessages();

	UFUNCTION(BlueprintCallable)
	void AddActionTurnMessage(const FString& NewMessage);
};
