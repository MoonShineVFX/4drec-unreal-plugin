// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFourdLZ4Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

class FOURDLZ4_API FFourdLZ4
{
public:
	static size_t GetDecompressedSize(const char* SrcBuffer, size_t SrcSize);
	static int32 Decompress(const char* SrcBuffer, size_t SrcSize, char* DstBuffer, size_t DstSize);
};