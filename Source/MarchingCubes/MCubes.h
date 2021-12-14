// (c) Henrique Hissa 2020

#pragma once

// Components that are included
#include "DrawDebugHelpers.h"
#include "ProceduralMeshComponent.h"
#include "RuntimeMeshActor.h"

#include "RuntimeMeshProviderStatic.h"

#include "Engine.h"

#include "Async/AsyncWork.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#include "CoreMinimal.h"
#include "MCParent.h"
#include "GameFramework/Actor.h"
#include "MCubes.generated.h"


class MarchingCubesAlgorithm : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<MarchingCubesAlgorithm>;
public:
	MarchingCubesAlgorithm(AMCubes* ActorMeshIn);

	~MarchingCubesAlgorithm();

	// Required
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(MarchingCubesAlgorithm, STATGROUP_ThreadPoolAsyncTasks);
	}

	AMCubes* ActorMesh;

	void DoWork();
};

UCLASS()
class MARCHINGCUBES_API AMCubes : public ARuntimeMeshActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "ProceduralMesh")
		class UProceduralMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)//, meta = (ClampMin = "0.1", UIMin = "0.1"))
		float SurfaceLevel = -2;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1", UIMin = "0.1", ClampMax = "10", UIMax = "10"))
		float PNoise1 = 1;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1", UIMin = "0.1", ClampMax = "100", UIMax = "100"))
		float PNoise2 = 20;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1", UIMin = "0.1", ClampMax = "100", UIMax = "100"))
		float PNoise3 = 10;

	UPROPERTY(EditAnywhere)
		bool bRenderDebug = true;

public:	
	// Sets default values for this actor's properties
	AMCubes();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Cancels thread
	virtual void BeginDestroy() override;
	// virtual void PostLoad() override;

public:	

	UPROPERTY(EditAnywhere, meta = (ClampMin = "2", UIMin = "2"))
		uint8 MeshXDimension = 9; //MINIMUM SIZE MUST BE 2 (2^3 = 8 possible points)

	UPROPERTY(EditAnywhere, meta = (ClampMin = "2", UIMin = "2"))
		uint8 MeshYDimension = 9; //MINIMUM SIZE MUST BE 2 (2^3 = 8 possible points)
		
	UPROPERTY(EditAnywhere, meta = (ClampMin = "2", UIMin = "2"))
		uint8 MeshZDimension = 9; //MINIMUM SIZE MUST BE 2 (2^3 = 8 possible points)
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CreateTriangle(/*const FVector& OriginPoint, const TArray<FVector>& VertexList*/);
	void BeginMarch();
	void UpdateMesh();
	FVector GetInterpolatedPosition(const FVector& P1, const FVector& P2, const float& V1, const float& V2) const;
	float DensityFunction(const FVector& Point) const;
	void CalculateTangentsForMeshCommented(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents);
	void FindVertOverlaps(int32 TestVertIndex, const TArray<FVector>& Verts, TArray<int32>& VertOverlaps);

	// Shared Variables
	TArray<uint16>* EdgeTable;
	TArray<FLookupCell>* TriTable;
	TArray<FVector>* IgnorePositions;
	class UMaterialInterface* MeshMaterial;
	uint8* RenderDistance;
	AMCParent* RootParent;
	TMap<FVector, AActor*>* ChunkList;
	FVector* ChunkOffset;
	uint16 MicroChunkResolution; 
	double* MinimumCutoff;
	double* MaximumCutoff;
	double* CutoffPower;
	double* BaseHeight;
	double* WalkHeight;
	double* SedimentWeight;
	double* SedimentFrequency;
	double* OverhangPresence;

	// Variables for threads
	AMCubes* Self;
	TArray<int> MeshTriIndices;
	FOccluderVertexArray MeshTris, MeshNorms;
	TArray<FRuntimeMeshTangent> MeshTans;
	TArray<FVector2D> MeshUVs;
	bool bBegunCalculations, bFinishedCalculations;

	URuntimeMeshProviderStatic* StaticProvider;

private:
	float CurrentSeconds;

	float FakeVolume;

	// TMap<FVector, float> MCPoints; // This map with vectors as keys will store the numbers which will determine a point's threshold
	
	float LastSurfaceLevel = 0, LastPNoise = 0;

	bool bAlreadyCreatedOnce = false;

	FAutoDeleteAsyncTask<MarchingCubesAlgorithm>* MCTask;

};