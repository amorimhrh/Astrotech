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
		bool bCreateOnlyOnce;

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
		uint8 ChunkSize = 8;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", UIMin = "1", ClampMax = "10000", UIMax = "10000"))
		uint16 MicroChunkResolution = 100; 

	UPROPERTY(EditAnywhere)
		FVector ChunkSpawnOffset;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		double MinimumCutoff;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		double MaximumCutoff;

	UPROPERTY(EditAnywhere)
		double CutoffPower;

	UPROPERTY(EditAnywhere)
		double BaseHeight;

	UPROPERTY(EditAnywhere)
		double WalkHeight;


	UPROPERTY(EditAnywhere)
		double SedimentWeight;

	UPROPERTY(EditAnywhere)
		double SedimentFrequency;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		double OverhangPresence;
	
public:	
	// Sets default values for this actor's properties
	AMCParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnMesh(const FVector& Location, const int32& MicroResolution);
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
	TMap<FVector, uint16> ChunkResolutions;
	TArray<FVector> OccupiedPositions;
	TArray<FVector> IgnorePositions;
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