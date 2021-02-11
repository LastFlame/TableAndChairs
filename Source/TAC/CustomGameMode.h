// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class UCustomShapeTemplateDataAsset;

UCLASS()
class TAC_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACustomGameMode(const FObjectInitializer& ObjectInitializer);	

public:
	const UCustomShapeTemplateDataAsset& GetCustomShapeTemplateData() const { return *CustomShapeTemplateData; }

private:
	UPROPERTY()
	UCustomShapeTemplateDataAsset* CustomShapeTemplateData;
};
