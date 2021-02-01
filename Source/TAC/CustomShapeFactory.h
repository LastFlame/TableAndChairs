// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomShapeFactory.generated.h"

class UCustomShapeTemplateDataAsset;

UCLASS()
class TAC_API UCustomShapeFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UCustomShapeFactory();

public:
	const UCustomShapeTemplateDataAsset& GetCustomShapeData() const { return *CustomShapeTemplateData; }
	
private:
	UPROPERTY()
	UCustomShapeTemplateDataAsset* CustomShapeTemplateData;
};
