// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShapeTemplateDataAsset.h"

UCustomShapeTemplateDataAsset::UCustomShapeTemplateDataAsset() :
	SphereRadius(8.0f), DragThreashold(5.0f),
	CustomTransform({ 0.0f, 0.0f, 73.0f }, { 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f, 1.5f }), ColliderMaxBoundOffset(20.0f), TableLegsSize(2.0f, 2.0f),
	TableMinSize(CustomTransform.Size.X, CustomTransform.Size.Y), DistanceBetweenChairs(10.0f), ChairDistanceFromTableSide(30.0f),
	ChairDistanceFromTableBottom(30.0f), ChairSize(20.0f, 18.0f, 1.0f), ChairLegsSize(1.0f, 1.0f), ChairBackRestSize(1.0f, 0.0f, 18.0f)
{
}
