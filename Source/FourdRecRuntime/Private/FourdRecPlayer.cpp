#include "FourdRecPlayer.h"

#include "FourdRecFrame.h"
#include "Engine/AssetManager.h"

#define FOURD_TEXTURE_SIZE 1024

// Sets default values
AFourdRecPlayer::AFourdRecPlayer()
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
void AFourdRecPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFourdRecPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsPlaying) return;
	
	const float TimeThreshold = 0.033;
	TimeLapsed += DeltaTime;
	if (TimeLapsed > TimeThreshold)
	{
		TimeLapsed -= TimeThreshold;
		UpdateMesh();
	}
}

void AFourdRecPlayer::PostLoad()
{
	Super::PostLoad();
	InitialMesh();
	UE_LOG(LogTemp, Warning, TEXT("Post Load*************"));
}

void AFourdRecPlayer::InitialMesh()
{
	if (FourdRecLoader == nullptr) return;

	if (MeshTex == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Create temp texture"));
		MeshTex = UTexture2D::CreateTransient(FOURD_TEXTURE_SIZE, FOURD_TEXTURE_SIZE, PF_DXT1);
		MeshTex->NeverStream = true;
		MeshTex->CompressionSettings = TC_Default;
#if WITH_EDITORONLY_DATA
		MeshTex->MipGenSettings = TMGS_NoMipmaps;
#endif
		MeshTex->UpdateResource();
	}

	if (DynMaterial == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Create temp dynamic material"));
		DynMaterial = UMaterialInstanceDynamic::Create(MeshMat, this);
		DynMaterial->SetTextureParameterValue("fourd_mat", MeshTex);
		FourdMesh->SetMaterial(0, DynMaterial);
	}

	UpdateMesh();
}

void AFourdRecPlayer::UpdateMesh()
{
	double start = FPlatformTime::Seconds();

	// Filter
	if (FourdMesh->GetNumSections() > 0) FourdMesh->ClearMeshSection(0);
	
	if (FourdRecLoader == nullptr) return;
	
	// Async load asset
	FSoftObjectPath ObjectPath = FourdRecLoader->FrameRefs[CurrentFrame];
	if (ObjectPath.IsNull()) return;
	
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestSyncLoad(ObjectPath);

	if (!Handle.IsValid()) return;
	
	UFourdRecFrame* FourdRecFrame = Cast<UFourdRecFrame>(Handle->GetLoadedAsset());
	UE_LOG(LogTemp, Display, TEXT("Load 4DREC frame"));
	
	// Initialize
	TArray<FVector> Vertices;
	TArray<FVector2D> UV;
	TArray<int32> Triangles;
    TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;

	FourdRecFrame->ApplyData(&Vertices, &UV, &Triangles, &Normals, &Tangents, &VertexColors, MeshTex);
	
	FourdMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV, VertexColors, Tangents, false);

	// Clean
	Handle->ReleaseHandle();

	// Profiler
	double end = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds."), end-start);

	CurrentFrame += 1;
	if (CurrentFrame >= FourdRecLoader->FrameRefs.Num()) CurrentFrame = 0;
}

void AFourdRecPlayer::StartPlay()
{
	bIsPlaying = true;
}
