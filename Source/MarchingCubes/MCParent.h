// (c) Henrique Hissa 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCParent.generated.h"

UCLASS()
class MARCHINGCUBES_API AMCParent : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* MeshMaterial;

	UPROPERTY(EditAnywhere)
		bool bCreateOnlyOne;

	UPROPERTY(EditAnywhere)
		FVector SingleChunkSpawnLocation;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "32", UIMax = "32"))
		uint8 RenderDistance;

	UPROPERTY(EditAnywhere)
		bool bLimitRenderHeight;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-16", UIMin = "-16", ClampMax = "0", UIMax = "0"))
		int8 RenderHeightMin = -16;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "16", UIMax = "16"))
		int8 RenderHeightMax = 16;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", UIMax = "32"))
		int32 ChunkSize = 8;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", UIMin = "1", ClampMax = "10000", UIMax = "10000"))
		int32 MicroChunkResolution = 100; 

	UPROPERTY(EditAnywhere)
		FVector ChunkSpawnOffset;

	UPROPERTY(EditAnywhere)
		int32 PlateauHeight;

	UPROPERTY(EditAnywhere)
		int32 PlateauTotalHeight;
		
	UPROPERTY(EditAnywhere)
		double PlateauIntensity;
		
	UPROPERTY(EditAnywhere)
		double PlateauBias;

	UPROPERTY(EditAnywhere)
		double SedimentaryWeight;

	UPROPERTY(EditAnywhere)
		double SedimentaryFrequency;
	
public:	
	// Sets default values for this actor's properties
	AMCParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnMesh(const FVector& Location);
	bool ArrayChanged(const TArray<float>& ArrayA, const TArray<float>& ArrayB) const;
	APlayerCameraManager* CamMgr;

	UPROPERTY(EditAnywhere)
		TArray<float> Frequencies;

	UPROPERTY(EditAnywhere)
		TArray<float> Amplitudes;


private:
	TArray<uint16> EdgeTable;
	TArray<FLookupCell> TriTable;
	TMap<FVector, AActor*> Chunks;
	TArray<FVector> OccupiedPositions;
	FVector CameraChunkPosition, CameraLastChunkPosition;
	TArray<float> LastFrequencies;
	TArray<float> LastAmplitudes;
	bool bArraysChanged;
	bool bFirstFrame;
	bool bAlreadyCreated;
};

USTRUCT()
struct FLookupCell
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int8> List;
};