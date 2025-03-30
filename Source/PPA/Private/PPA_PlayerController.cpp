// Fill out your copyright notice in the Description page of Project Settings.


#include "PPA_PlayerController.h"
#include "Components/InputComponent.h"

APPA_PlayerController::APPA_PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void APPA_PlayerController::ClickOnGrid()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Click rilevato!"));
	const auto HumanPlayer = Cast<APPA_HumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->OnClick();
	}
}

void APPA_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Adding Input Mapping Context"));
		Subsystem->AddMappingContext(TTTContext, 0);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BeginPlay: Subsystem not found!"));
	}
}

void APPA_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Click rilevato!"));
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Click rilevato!"));
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &APPA_PlayerController::ClickOnGrid);
	}
}
