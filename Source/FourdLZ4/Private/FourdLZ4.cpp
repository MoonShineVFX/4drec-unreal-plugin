// Copyright Epic Games, Inc. All Rights Reserved.
#include "FourdLZ4.h"

// #define LZ4F_STATIC_LINKING_ONLY
// #define LZ4F_PUBLISH_STATIC_FUNCTIONS
#include "lz4frame.h"

#define LOCTEXT_NAMESPACE "FFourdLZ4Module"

void FFourdLZ4Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FFourdLZ4Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

size_t FFourdLZ4::GetDecompressedSize(const char* SrcBuffer, size_t SrcSize)
{
	LZ4F_dctx* Dctx{};
	LZ4F_createDecompressionContext(&Dctx, LZ4F_getVersion());
	LZ4F_frameInfo_t FrameInfo{};
	LZ4F_getFrameInfo(Dctx, &FrameInfo, SrcBuffer, &SrcSize);
	const size_t DecompressedSize = FrameInfo.contentSize;
	LZ4F_freeDecompressionContext(Dctx);
	return DecompressedSize;
}

int32 FFourdLZ4::Decompress(const char* SrcBuffer, size_t SrcSize, char* DstBuffer, size_t DstSize)
{
	LZ4F_dctx* Dctx{};
	LZ4F_createDecompressionContext(&Dctx, LZ4F_getVersion());
	const LZ4F_errorCode_t ErrorCode = LZ4F_decompress(
            Dctx,
            DstBuffer, &DstSize,
            SrcBuffer, &SrcSize,
            nullptr);
	LZ4F_freeDecompressionContext(Dctx);
	return ErrorCode;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFourdLZ4Module, FourdLZ4)