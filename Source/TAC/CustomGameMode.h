// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class UCustomCollisionSystemContainer;

UCLASS()
class TAC_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACustomGameMode(const FObjectInitializer& ObjectInitializer);	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

private:
	UPROPERTY();
	UCustomCollisionSystemContainer* CustomRaycastSystemContainer;
};
