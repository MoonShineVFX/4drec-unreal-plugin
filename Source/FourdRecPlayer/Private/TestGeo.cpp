#include "TestGeo.h"
#include "Containers/UnrealString.h"
#include "FourdLZ4.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

// Sets default values
ATestGeo::ATestGeo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT_TRS"));

	FourdMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("4DREC_MESH"));
	FourdMesh->SetFlags(RF_Transient);
	FourdMesh->bUseAsyncCooking = true;
	FourdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FourdMesh->ClearCollisionConvexMeshes();

	FourdMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ATestGeo::BeginPlay()
{
	Super::BeginPlay();
	// UpdateMesh();
}

// Called every frame
void ATestGeo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestGeo::PostLoad()
{
	Super::PostLoad();
	UpdateMesh();
}

void ATestGeo::PostActorCreated()
{
	Super::PostActorCreated();
	UpdateMesh();
}

void ATestGeo::UpdateMesh()
{
	if (FourdMesh->GetNumSections() > 0) FourdMesh->ClearMeshSection(0);
	// Get compressed data
	TArray<uint8> FileData;
	const FString FourdPath = "C:\\Users\\moonshine\\Desktop\\testload_one\\001696.4df"; 
	FFileHelper::LoadFileToArray(FileData, *FourdPath);
	const int32 SrcSize = *reinterpret_cast<int32*>(FileData.GetData() + 64);
	const int32 TexSize = *reinterpret_cast<int32*>(FileData.GetData() + 68);
	UE_LOG(LogTemp, Display, TEXT("File size is %d, geo size is %d, tex size is %d"), FileData.Num(), SrcSize, TexSize);

	// Meta
	FString FormatMeta;
	FFileHelper::BufferToString(FormatMeta, FileData.GetData(), 4);
	UE_LOG(LogTemp, Display, TEXT("Format is %s"), *FormatMeta);

	// Geo
	const char* SrcPointer = reinterpret_cast<char*>(FileData.GetData() + 1024);

	const int32 DstSize = FFourdLZ4::GetDecompressedSize(SrcPointer, SrcSize);
	UE_LOG(LogTemp, Display, TEXT("Compressed bytes is %d"), SrcSize);
	UE_LOG(LogTemp, Display, TEXT("Uncompressed bytes is %d"), DstSize);
	
	char* DstPointer = (char*)FMemory::Malloc(DstSize);
	int32 ErrorCode = FFourdLZ4::Decompress(SrcPointer, SrcSize, DstPointer, DstSize);
	UE_LOG(LogTemp, Display, TEXT("Error code is %d"), (int32)ErrorCode);
	float* DstFloatPointer = reinterpret_cast<float*>(DstPointer);
	UE_LOG(LogTemp, Display, TEXT("first pos is %f, %f, %f"),
		*DstFloatPointer,
		*(DstFloatPointer + 1),
		*(DstFloatPointer + 2));
	UE_LOG(LogTemp, Display, TEXT("first uv is %f, %f"),
		*(DstFloatPointer + 3),
		*(DstFloatPointer + 4));

	// Texture
	const char* TexPointer = reinterpret_cast<char*>(FileData.GetData() + 1024 + SrcSize);
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(TexPointer, TexSize))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, (int32)8, UncompressedBGRA))
		{
			MeshTex = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
			MeshTex->MipGenSettings = TMGS_NoMipmaps;
			void* TextureData = MeshTex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			MeshTex->PlatformData->Mips[0].BulkData.Unlock();
 
			// Update the rendering resource from data.
			MeshTex->UpdateResource();
		}
	}
     
	
	// Apply data
	const int32 VerticesCount = DstSize / sizeof(float) / 5;
	TArray<FVector> Vertices;
	TArray<FVector2D> UV0;
	TArray<int32> Triangles;
	Vertices.AddUninitialized(VerticesCount);
	UV0.AddUninitialized(VerticesCount);
	Triangles.AddUninitialized(VerticesCount);
	
	for (int i = 0; i < VerticesCount; i++)
	{
		FMemory::Memcpy(Vertices.GetData() + i,
			DstPointer + 5 * 4 * i,
			sizeof(FVector));
		Vertices[i] *= 100;
		const float tempY = Vertices[i].Y;
		Vertices[i].Y = Vertices[i].Z;
		Vertices[i].Z = tempY;
		
		FMemory::Memcpy(UV0.GetData() + i,
            DstPointer + 5 * 4 * i + 12,
            sizeof(FVector2D));

		Triangles[i] = i;
	}

	UE_LOG(LogTemp, Display, TEXT("Vertices[0] are %f, %f, %f"),
    Vertices[0].X, Vertices[0].Y, Vertices[0].Z);
	
	TArray<FVector> Normals;
	Normals.SetNum(VerticesCount);
	TArray<FProcMeshTangent> Tangents;
	Tangents.SetNum(VerticesCount);
	TArray<FLinearColor> VertexColors;
	VertexColors.SetNum(VerticesCount);

	for (int i = 0; i < VerticesCount / 3; i++)
	{
		FVector A = Vertices[i * 3 + 2];
		FVector B = Vertices[i * 3 + 1];
		FVector C = Vertices[i * 3];
		const FVector N = FVector::CrossProduct(B - A, C - A).GetUnsafeNormal();
		Normals[i * 3] = N;
		Normals[i * 3 + 1] = N;
		Normals[i * 3 + 2] = N;

		FVector2D aUV = UV0[i * 3];
		FVector2D bUV = UV0[i * 3 + 1];
		
		FVector surfaceTangent = (
			(bUV.X - aUV.X) / FVector2D::Distance(bUV, aUV) * (B - A) +
			(bUV.Y - aUV.Y) / FVector2D::Distance(bUV, aUV) * FVector::CrossProduct(N, B - A)
			).GetSafeNormal();
		const FProcMeshTangent procTangent = FProcMeshTangent(surfaceTangent, true);
 
		Tangents[i * 3] = procTangent;
		Tangents[i * 3 + 1] = procTangent;
		Tangents[i * 3 + 2] = procTangent;
	}
	
	FourdMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);
	FourdMesh->SetMaterial(0, MeshMat);

	// Free
	FMemory::Free(DstPointer);
}
