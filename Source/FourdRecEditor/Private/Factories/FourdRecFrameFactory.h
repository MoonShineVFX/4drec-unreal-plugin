﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Factories/Factory.h"
#include "CoreMinimal.h"
#include "FourdRecFrameFactory.generated.h"

/**
 * 
 */
UCLASS()
class FOURDRECEDITOR_API UFourdRecFrameFactory : public UFactory
{
	GENERATED_BODY()

public:
	UFourdRecFrameFactory();
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};
