#include "FourdRecFrameFactory.h"

#include "AssetRegistryModule.h"
#include "FourdRecFrame.h"
#include "FourdRecLoader.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "LZ4wrapper.h"
#include "TextureCompressorModule.h"
#include "ProceduralMeshComponent.h"
#include "ImageCore.h"

#define FOURDREC_HEADER_GEO_SIZE_OFFSET 64
#define FOURDREC_HEADER_TEX_SIZE_OFFSET 68
#define FOURDREC_HEADER_TOTAL_OFFSET 1024


UFourdRecFrameFactory::UFourdRecFrameFactory() : Super()
{
	Formats.Add(FString(TEXT("4df;")) + NSLOCTEXT("UFourdRecFrameFactory", "4DRecFrame", "4DRec Frame").ToString());
	SupportedClass = UFourdRecFrame::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}


UObject* UFourdRecFrameFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString ImportFolder = FPaths::GetPath(Filename);
	TArray<FString> ImportFiles;
	IFileManager::Get().FindFiles(ImportFiles, *ImportFolder, TEXT("4DF"));
	
	TArray<UFourdRecFrame*> GeneratedFrames;
	for (int i = 0; i < ImportFiles.Num(); i++)
	{
		FString ImportFile = FPaths::Combine(ImportFolder, ImportFiles[i]);
		FString ImportFileName = FPaths::GetBaseFilename(ImportFiles[i]);
		UE_LOG(LogTemp, Warning, TEXT("Import 4D frame file: %s"), *ImportFile);
		
		// Get compressed data
		TArray<uint8> FileData;
		FFileHelper::LoadFileToArray(FileData, *ImportFile);
		const int32 GeoSrcSize = *reinterpret_cast<int32*>(FileData.GetData() + FOURDREC_HEADER_GEO_SIZE_OFFSET);
		const int32 TexSize = *reinterpret_cast<int32*>(FileData.GetData() + FOURDREC_HEADER_TEX_SIZE_OFFSET);
		UE_LOG(LogTemp, Display, TEXT("File size is %d, geo size is %d, tex size is %d"), FileData.Num(), GeoSrcSize, TexSize);

		// Meta
		FString FormatMeta;
		FFileHelper::BufferToString(FormatMeta, FileData.GetData(), 4);
		UE_LOG(LogTemp, Display, TEXT("Format is %s"), *FormatMeta);

		// Geo
		const char* SrcPointer = reinterpret_cast<char*>(FileData.GetData() + FOURDREC_HEADER_TOTAL_OFFSET);

		const int32 GeoDstSize = FLZ4wrapper::GetDecompressedSize(SrcPointer, GeoSrcSize);
		UE_LOG(LogTemp, Display, TEXT("Geo compressed bytes is %d"), GeoSrcSize);
		UE_LOG(LogTemp, Display, TEXT("Geo decompressed bytes is %d"), GeoDstSize);
	
		char* GeoDstPointer = static_cast<char*>(FMemory::Malloc(GeoDstSize));
		int32 ErrorCode = FLZ4wrapper::Decompress(SrcPointer, GeoSrcSize, GeoDstPointer, GeoDstSize);
		UE_LOG(LogTemp, Display, TEXT("Geo error code is %d"), ErrorCode);

		// Texture
		TArray<FCompressedImage2D> CompressedMips;
		int TextureWidth = -1;
		int TextureHeight = -1;
	
		const char* TexPointer = reinterpret_cast<char*>(FileData.GetData() + FOURDREC_HEADER_TOTAL_OFFSET + GeoSrcSize);
	
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(TexPointer, TexSize))
		{
			TArray<uint8> DecompressedRaw;
			if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, DecompressedRaw))
			{
				UE_LOG(LogTemp, Display, TEXT("Decompressed image size is %d"), DecompressedRaw.Num());
				TextureWidth = ImageWrapper->GetWidth();
				TextureHeight = ImageWrapper->GetHeight();

				// Compress Texture
				ITextureCompressorModule& Compressor = FModuleManager::LoadModuleChecked<ITextureCompressorModule>(TEXTURE_COMPRESSOR_MODULENAME);
				TArray<FImage> SourceMips;
				TArray<FImage> NormalSourceMips;
				FTextureBuildSettings BuildSettings;
				uint32 MipNum;
				uint32 ExtData;

				FImage RawImage = FImage(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(),
                    ERawImageFormat::BGRA8, EGammaSpace::Linear);
				RawImage.RawData = DecompressedRaw;
				FImage ResizedImage;
				RawImage.ResizeTo(ResizedImage, 1024, 1024, RawImage.Format, RawImage.GammaSpace);
				
				SourceMips.Add(ResizedImage);
				BuildSettings.TextureFormatName = TEXT("DXT1");
				BuildSettings.MipGenSettings = TMGS_NoMipmaps;

				UE_LOG(LogTemp, Display, TEXT("Begin compressing texture to DXT1"))
				if (Compressor.BuildTexture(SourceMips, NormalSourceMips, BuildSettings, CompressedMips, MipNum, ExtData))
				{
					UE_LOG(LogTemp, Display, TEXT("Compressed success"))
				}
			}
		}
		UE_LOG(LogTemp, Display, TEXT("Compressed image size is %d"), CompressedMips[0].RawData.Num());
	
		// Geo
		const int GeoElementSize = sizeof(FVector) + sizeof(FVector2D);
		const int32 VerticesCount = GeoDstSize / GeoElementSize;
		TArray<FVector> Vertices;
		TArray<FVector2D> UV0;
		Vertices.AddUninitialized(VerticesCount);
		UV0.AddUninitialized(VerticesCount);

		for (int v = 0; v < VerticesCount; v++)
		{
			FMemory::Memcpy(Vertices.GetData() + v,
                GeoDstPointer + GeoElementSize * v,
                sizeof(FVector));
		
			Vertices[v] *= 100; // World scale

			// Flip Y/Z
			const float TempY = Vertices[v].Y;
			Vertices[v].Y = Vertices[v].Z;
			Vertices[v].Z = TempY;
		
			FMemory::Memcpy(UV0.GetData() + v,
                GeoDstPointer + GeoElementSize * v + sizeof(FVector),
                sizeof(FVector2D));
		}

		// Free
		FMemory::Free(GeoDstPointer);

		// Create asset
		UE_LOG(LogTemp, Warning, TEXT("ImportFileName: %s"), *ImportFileName);
		const FString PackagePath = FString("/Game/4DRecShot/") + ImportFileName;
		UPackage* ThisPackage = CreatePackage(nullptr, *PackagePath);
		UFourdRecFrame* Frame = NewObject<UFourdRecFrame>(ThisPackage, InClass, *ImportFileName, Flags);
		Frame->SaveData(VerticesCount, &Vertices, &UV0, CompressedMips[0].RawData.GetData(),
            CompressedMips[0].RawData.Num(), TextureWidth, TextureHeight);
		// ReSharper disable once CppExpressionWithoutSideEffects
		Frame->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Frame);
		FString SaveFilePath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
		bool bSaveResult = UPackage::SavePackage(ThisPackage, Frame, Flags, *SaveFilePath); 
		GeneratedFrames.Add(Frame);
	}

	// Create meta
	UFourdRecLoader* Loader = NewObject<UFourdRecLoader>(InParent, UFourdRecLoader::StaticClass(), FName("TestLoader"), Flags);
	Loader->StartFrame = FCString::Atoi(*GeneratedFrames[0]->GetFName().ToString());
	Loader->EndFrame = FCString::Atoi(*GeneratedFrames[GeneratedFrames.Num() - 1]->GetFName().ToString());
	for (int i = 0; i < GeneratedFrames.Num(); i++)
	{
		FSoftObjectPath ObjectPath = FSoftObjectPath(GeneratedFrames[i]);
		Loader->FrameRefs.Add(ObjectPath);
	}
	
	// Free
	bOutOperationCanceled = false;
	return Loader;
}
