// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUnit.h"
#include "Brawler.generated.h"

/**
 * 
 */
UCLASS()
class PPA_API ABrawler : public ABaseUnit
{
	GENERATED_BODY()

public:
	ABrawler();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
};
