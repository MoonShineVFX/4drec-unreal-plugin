// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FLZ4wrapper
{
public:
	static size_t GetDecompressedSize(const char* SrcBuffer, size_t SrcSize);
	static int32 Decompress(const char* SrcBuffer, size_t SrcSize, char* DstBuffer, size_t DstSize);
};