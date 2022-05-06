// (c) Henrique Hissa 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCParent.generated.h"

UCLASS()
class MARCHINGCUBES_API AMCParent : public AActor
{
	GENERATED_BODY()

	/** Determines the material applied to the generated chunks. */ UPROPERTY(EditAnywhere)
		class UMaterialInterface* MeshMaterial;

	/** Only generate terrain once? */ UPROPERTY(EditAnywhere, meta = (DisplayName = "Generate once?"))
		bool bCreateOnlyOnce;

	/** Only create a single chunk? */ UPROPERTY(EditAnywhere, meta = (DisplayName = "Only one chunk?"))
		bool bCreateOnlyOne;

	/** Where to create single chunk. */ UPROPERTY(EditAnywhere, meta = (EditCondition = "bCreateOnlyOne"))
		FVector SingleChunkSpawnLocation;

	/** How far away chunks are generated. */ UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "32", UIMax = "32"))
		uint8 RenderDistance;

	/** Should you ignore certain z values when generating chunks? */ UPROPERTY(EditAnywhere)
		bool bLimitRenderHeight;

	/** Doesn't generate chunks below this z value. */ UPROPERTY(EditAnywhere, meta = (EditCondition = "bLimitRenderHeight", ClampMin = "-16", UIMin = "-16", ClampMax = "0", UIMax = "0"))
		int8 RenderHeightMin = -16;

	/** Doesn't generate chunks above this z value. */ UPROPERTY(EditAnywhere, meta = (EditCondition = "bLimitRenderHeight", ClampMin = "0", UIMin = "0", ClampMax = "16", UIMax = "16"))
		int8 RenderHeightMax = 16;

	/** How many 100-units large is a chunk. */ UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", UIMax = "32"))
		uint8 ChunkSize = 8;

	/** How large a single march is. */ UPROPERTY(EditAnywhere, meta = (ClampMin = "1", UIMin = "1", ClampMax = "10000", UIMax = "10000"))
		uint16 MicroChunkResolution = 100; 

	/** Offsets camera chunk location by this amount. */ UPROPERTY(EditAnywhere)
		FVector ChunkSpawnOffset;

	/** Lowest value of flat-terrain perlin noise to consider.
	 * 
	 *  Min = Epsilon and Max = 0 completely deactivates flat terrains.
	 *  Min = 1 and Max =/= 1 completely deactivates mountainous terrains.  */ UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		double MinimumCutoff;

	/** Highest value of flat-terrain perlin noise to consider. 
	 * 
	 *  Min = Epsilon and Max = 0 completely deactivates flat terrains.
	 *  Min = 1 and Max =/= 1 completely deactivates mountainous terrains.  */ UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		double MaximumCutoff;

	/** How smooth the transition between masked and unmasked parts is. */ UPROPERTY(EditAnywhere)
		double CutoffPower;

	/** Translates terrain in z-axis. */ UPROPERTY(EditAnywhere)
		double BaseHeight;

	/** Defines flat terrain height. */ UPROPERTY(EditAnywhere)
		double WalkHeight;


	/** Sediment noise amplitude. */ UPROPERTY(EditAnywhere, meta = (DisplayName = "Sediment Amplitude"))
		double SedimentWeight;

	/** Sediment noise frequency. */ UPROPERTY(EditAnywhere)
		double SedimentFrequency;

	/** How strong the subtractive 3D perlin noise is on the flat terrain. */ UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		double OverhangPresence;

	/** Get information about created and finished chunks. **/ UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
		void ChunkProgressInfo(int32& OutCreatedChunks, int32& OutFinishedChunks);
	
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

	/** Must contain 11 variables. Frequency multiplies coordinates.
	 * 
	 *  0: Base noise frequency. 
	 *  1-7: Mountainous Perlin noise frequency. 
	 *  8: Mountainous domain-warped Perlin noise frequency.
	 *  9: Subtractive 3D Perlin noise frequency.
	 *  10: Walking mask noise frequency. */ UPROPERTY(EditAnywhere)
		TArray<float> Frequencies;

	/** Must contain 11 variables. Putting it at 0 deactivates the layer of noise.
	 * 
	 *  0: Base noise amplitude. 
	 *  1-7: Mountainous Perlin noise amplitude. 
	 *  8: Mountainous domain-warped Perlin noise amplitude.
	 *  9: Subtractive 3D Perlin noise amplitude.
	 *  10: Walking mask noise amplitude. */ UPROPERTY(EditAnywhere)
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
	int32 NumChunksCreated, NumChunksFinished;
};

USTRUCT()
struct FLookupCell
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int8> List;
};