// Copyright Epic Games, Inc. All Rights Reserved.
#include "LZ4wrapper.h"
#include "lz4frame.h"

size_t FLZ4wrapper::GetDecompressedSize(const char* SrcBuffer, size_t SrcSize)
{
	LZ4F_dctx* Dctx{};
	LZ4F_createDecompressionContext(&Dctx, LZ4F_getVersion());
	LZ4F_frameInfo_t FrameInfo{};
	LZ4F_getFrameInfo(Dctx, &FrameInfo, SrcBuffer, &SrcSize);
	const size_t DecompressedSize = FrameInfo.contentSize;
	LZ4F_freeDecompressionContext(Dctx);
	return DecompressedSize;
}

int32 FLZ4wrapper::Decompress(const char* SrcBuffer, size_t SrcSize, char* DstBuffer, size_t DstSize)
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
