// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUnit.h"
#include "Sniper.generated.h"

/**
 * 
 */
UCLASS()
class PPA_API ASniper : public ABaseUnit
{
	GENERATED_BODY()
public:
	ASniper();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

