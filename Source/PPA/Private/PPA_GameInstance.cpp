// Fill out your copyright notice in the Description page of Project Settings.


#include "PPA_GameInstance.h"

bool UPPA_GameInstance::GetIsBrawlerSpawned()
{
	return bIsBrawlerSpawned;
}

bool UPPA_GameInstance::GetIsSniperSpawned()
{
	return bIsSniperSpawned;
}


int32 UPPA_GameInstance::GetHumanSniperHealth()
{
	return HumanSniperHealth;
}

int32 UPPA_GameInstance::GetHumanBrawlerHealth()
{
	return HumanBrawlerHealth;
}

int32 UPPA_GameInstance::GetAiSniperHealth()
{
	return AiSniperHealth;
}

int32 UPPA_GameInstance::GetAiBrawlerHealth()
{
	return AiBrawlerHealth;
}


FString UPPA_GameInstance::GetTurnMessage()
{
	return CurrentTurnMessage;
}

void UPPA_GameInstance::SetTurnMessage(FString Message)
{
	CurrentTurnMessage = Message;
}

TArray<FString> UPPA_GameInstance::GetActionTurnMessages()
{
	return ActionTurnMessages;
}


void UPPA_GameInstance::AddActionTurnMessage(const FString& NewMessage)
{
	ActionTurnMessages.Add(NewMessage);

}
