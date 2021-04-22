#include "FourdRecFrame.h"
#include "Misc/Compression.h"


void UFourdRecFrame::SaveData(int InVerticesCount, const TArray<FVector>* InVertices, const TArray<FVector2D>* InUV, const void* InTextureData, int InTextureSize, int InTextureWidth, int InTextureHeight)
{
	// initial
	VerticesCount = InVerticesCount;
	const int VerticesSize = InVerticesCount * sizeof(FVector);
	const int UVSize = InVerticesCount * sizeof(FVector2D);
	GeoDecompressedSize = VerticesSize + UVSize;

	// LZ4 compression
	void* LZ4DecompressedData = FMemory::Malloc(GeoDecompressedSize);
	FMemory::Memcpy(LZ4DecompressedData, InVertices->GetData(), VerticesSize);
	FMemory::Memcpy(static_cast<uint8*>(LZ4DecompressedData) + VerticesSize, InUV->GetData(), UVSize);
	int32 LZ4CompressedSize = GeoDecompressedSize;
	void* LZ4CompressedData = FMemory::Malloc(GeoDecompressedSize);
	FCompression::CompressMemory(NAME_LZ4, LZ4CompressedData, LZ4CompressedSize, LZ4DecompressedData, GeoDecompressedSize);

	// Apply geo data
	GeoCompressedData.AddUninitialized(LZ4CompressedSize);
	GeoCompressedSize = LZ4CompressedSize;
	FMemory::Memcpy(GeoCompressedData.GetData(), LZ4CompressedData, LZ4CompressedSize);
	FMemory::Free(LZ4DecompressedData);
	FMemory::Free(LZ4CompressedData);

	// Apply texture
	TextureSize = InTextureSize;
	TextureWidth = InTextureWidth;
	TextureHeight = InTextureHeight;
	TextureData.AddUninitialized(InTextureSize);
	FMemory::Memcpy(TextureData.GetData(), InTextureData, InTextureSize);
}

void UFourdRecFrame::ApplyData(TArray<FVector>* OutVertices, TArray<FVector2D>* OutUV, TArray<int32>* OutTriangles,
	TArray<FVector>* OutNormals, TArray<FProcMeshTangent>* OutTangents, TArray<FLinearColor>* OutVertexColors, UTexture2D* OutTexture)
{
	// initial
	OutVertices->AddUninitialized(VerticesCount);
	OutUV->AddUninitialized(VerticesCount);
	OutTriangles->AddUninitialized(VerticesCount);
	// OutNormals->AddUninitialized(VerticesCount);
	// OutTangents->AddUninitialized(VerticesCount);
	OutVertexColors->SetNum(VerticesCount);

	// LZ4 compression
	const int VerticesSize = VerticesCount * sizeof(FVector);
	const int UVSize = VerticesCount * sizeof(FVector2D);
	void* LZ4DecompressedBuffer = FMemory::Malloc(VerticesSize + UVSize);
	FCompression::UncompressMemory(NAME_LZ4, LZ4DecompressedBuffer, GeoDecompressedSize,
		GeoCompressedData.GetData(), GeoCompressedSize);
	FMemory::Memcpy(OutVertices->GetData(), LZ4DecompressedBuffer, VerticesSize);
	FMemory::Memcpy(OutUV->GetData(), static_cast<uint8*>(LZ4DecompressedBuffer) + VerticesSize, UVSize);
	FMemory::Free(LZ4DecompressedBuffer);

	// Triangles
	for (int i = 0; i < VerticesCount; i++)
	{
		(*OutTriangles)[i] = i;
	}

	// Normal and tangents
	OutNormals->AddZeroed(VerticesCount);
	OutTangents->AddZeroed(VerticesCount);
	// for (int i = 0; i < VerticesCount / 3; i++)
	// {
	// 	FVector N_A = (*OutNormals)[i * 3 + 2];
	// 	FVector N_B = (*OutNormals)[i * 3 + 1];
	// 	FVector N_C = (*OutNormals)[i * 3];
	// 	const FVector N = FVector::CrossProduct(N_B - N_A, N_C - N_A).GetUnsafeNormal();
	// 	(*OutNormals)[i * 3] = N;
	// 	(*OutNormals)[i * 3 + 1] = N;
	// 	(*OutNormals)[i * 3 + 2] = N;
 //
	// 	FVector2D UV_A = (*OutUV)[i * 3];
	// 	FVector2D UV_B = (*OutUV)[i * 3 + 1];
 //
	// 	const FVector SurfaceTangent = (
 //            (UV_B.X - UV_A.X) / FVector2D::Distance(UV_B, UV_A) * (N_B - N_A) +
 //            (UV_B.Y - UV_A.Y) / FVector2D::Distance(UV_B, UV_A) * FVector::CrossProduct(N, N_B - N_A)
 //            ).GetSafeNormal();
	// 	const FProcMeshTangent ProcTangent = FProcMeshTangent(SurfaceTangent, true);
 //
	// 	(*OutTangents)[i * 3] = ProcTangent;
	// 	(*OutTangents)[i * 3 + 1] = ProcTangent;
	// 	(*OutTangents)[i * 3 + 2] = ProcTangent;
	// }

	// Texture
	void* MipData = OutTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(MipData, TextureData.GetData(), OutTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());
	OutTexture->PlatformData->Mips[0].BulkData.Unlock();
	OutTexture->UpdateResource();
}
