#pragma once

#include "CoreMinimal.h"

#include "FourdRecLoader.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "FourdRecPlayer.generated.h"


UCLASS()
class AFourdRecPlayer : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	bool bIsPlaying = false;

	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	float TimeLapsed = 0;
	
	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	UTexture2D* MeshTex;

	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	UProceduralMeshComponent* FourdMesh;

public:	
	AFourdRecPlayer();

	UFUNCTION(Category="FourdRec")
	void InitialMesh();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category="FourdRec")
	void UpdateMesh();

	UFUNCTION(BlueprintCallable, Category="FourdRec")
    void StartPlay();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FourdRec")
	int CurrentFrame = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FourdRec")
	UMaterialInterface* MeshMat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="FourdRec")
	UFourdRecLoader* FourdRecLoader = nullptr;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void PostLoad() override;
};
