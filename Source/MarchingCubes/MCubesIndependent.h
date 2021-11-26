// (c) Henrique Hissa 2020-2021

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

#include "GameFramework/Actor.h"
#include "MCubesIndependent.generated.h"


class MarchingCubesAlgorithmIndependent : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<MarchingCubesAlgorithmIndependent>;
public:
	MarchingCubesAlgorithmIndependent(AMCubesIndependent* ActorMeshIn);

	~MarchingCubesAlgorithmIndependent();

	// Required
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(MarchingCubesAlgorithmIndependent, STATGROUP_ThreadPoolAsyncTasks);
	}

	AMCubesIndependent* ActorMesh;

	void DoWork();
};

UCLASS()
class MARCHINGCUBES_API AMCubesIndependent : public ARuntimeMeshActor
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
	AMCubesIndependent();
	
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
	
	UPROPERTY(EditAnywhere)
		class UMaterialInterface* MeshMaterial;
		
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CreateTriangle(/*const FVector& OriginPoint, const TArray<FVector>& VertexList*/);
	void BeginMarch();
	FVector GetInterpolatedPosition(const FVector& P1, const FVector& P2, const float& V1, const float& V2) const;
	float DensityFunction(const FVector& Point) const;

	// Essential Variables

	// Variables for threads
	AMCubesIndependent* Self;
	TArray<int> MeshTriIndices;
	FOccluderVertexArray MeshTris, MeshNorms;
	TArray<FRuntimeMeshTangent> MeshTans;
	TArray<FVector2D> MeshUVs;
	bool bBegunCalculations, bFinishedCalculations;

	URuntimeMeshProviderStatic* StaticProviderIndependent = nullptr;

private:
	float CurrentSeconds;

	float FakeVolume;

	TArray<uint16> EdgeTable;
	TArray<FLookupCellIndependent> TriTable;

	// TMap<FVector, float> MCPoints; // This map with vectors as keys will store the numbers which will determine a point's threshold
	
	float LastSurfaceLevel = 0, LastPNoise = 0;

	FAutoDeleteAsyncTask<MarchingCubesAlgorithmIndependent>* MCTask;

};

USTRUCT()
struct FLookupCellIndependent
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int8> List;
};