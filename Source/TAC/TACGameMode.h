// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TAC/TAC.h"
#include "GameFramework/GameModeBase.h"
#include "TACGameMode.generated.h"

class UTACShapesTemplateData;

UCLASS()
class TAC_API ATACGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATACGameMode(const FObjectInitializer& ObjectInitializer);	
};
