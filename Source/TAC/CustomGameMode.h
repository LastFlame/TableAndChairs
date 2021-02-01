// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class UCustomCollisionSystemContainer;
class UCustomShapeTemplateDataAsset;

UCLASS()
class TAC_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACustomGameMode(const FObjectInitializer& ObjectInitializer);	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	const UCustomShapeTemplateDataAsset& GetCustomShapeTemplateData() const { return *CustomShapeTemplateData; }

private:
	UPROPERTY();
	UCustomCollisionSystemContainer* CustomRaycastSystemContainer;

	UPROPERTY()
	UCustomShapeTemplateDataAsset* CustomShapeTemplateData;
};
