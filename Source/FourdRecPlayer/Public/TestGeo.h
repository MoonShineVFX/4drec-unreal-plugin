#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "TestGeo.generated.h"


UCLASS()
class ATestGeo : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestGeo();

	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	UProceduralMeshComponent* FourdMesh;
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void UpdateMesh();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterial* MeshMat;

	UPROPERTY(Transient, DuplicateTransient, NonPIEDuplicateTransient)
	UTexture2D* MeshTex;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

};
