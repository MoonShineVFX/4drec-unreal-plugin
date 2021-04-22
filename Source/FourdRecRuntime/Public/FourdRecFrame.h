// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "FourdRecFrame.generated.h"

/**
 * 
 */
UCLASS()
class FOURDRECRUNTIME_API UFourdRecFrame : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	void SaveData(int InVerticesCount, const TArray<FVector>* InVertices, const TArray<FVector2D>* InUV,
		const void* InTextureData, int InTextureSize, int InTextureWidth, int InTextureHeight);
	void ApplyData(TArray<FVector>* OutVertices, TArray<FVector2D>* OutUV, TArray<int32>* OutTriangles,
		TArray<FVector>* OutNormals, TArray<FProcMeshTangent>* OutTangents, TArray<FLinearColor>* OutVertexColors, UTexture2D* OutTexture);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int VerticesCount;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int TextureWidth;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int TextureHeight;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int TextureSize;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int GeoCompressedSize;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int GeoDecompressedSize;

	// binary data
	UPROPERTY()
	TArray<uint8> GeoCompressedData;
	UPROPERTY()
	TArray<uint8> TextureData;
};
