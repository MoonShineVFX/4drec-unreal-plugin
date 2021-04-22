// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FourdRecFrame.h"
#include "UObject/Object.h"
#include "FourdRecLoader.generated.h"

/**
 * 
 */
UCLASS()
class FOURDRECRUNTIME_API UFourdRecLoader : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int StartFrame;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int EndFrame;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FSoftObjectPath> FrameRefs;
};
