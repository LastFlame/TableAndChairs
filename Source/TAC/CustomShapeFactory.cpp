// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShapeFactory.h"
#include "CustomShapeTemplateDataAsset.h"
#include "TAC/Actors/CustomShape.h"

UCustomShapeFactory::UCustomShapeFactory()
{
	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != NULL)
	{
		CustomShapeTemplateData = CustomShapeDataAsset.Object;
	}
}
