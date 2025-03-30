// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PPA_PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPPA_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PPA_API IPPA_PlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	int32 PlayerNumber;

	virtual void OnTurn() {};
	virtual void OnWin() {};
	virtual void OnLose() {};
	virtual void OnUnitMovement(FVector2D StartPos, int32 MaxDistance, int32 AttackRange, bool IsHuman) {};
};
