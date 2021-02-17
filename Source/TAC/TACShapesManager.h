// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TAC/TACShapesTemplateData.h"
#include "TACShapesManager.generated.h"

UCLASS()
class TAC_API UTACShapesManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UTACShapesManager();

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	class ATACTableShape* CreateTable(const FVector& Location) const;

public:
	const UTACShapesTemplateData& GetShapesTemplateData() const { return *ShapesTemplateData; }

private:
	UPROPERTY()
	UTACShapesTemplateData* ShapesTemplateData;
};
