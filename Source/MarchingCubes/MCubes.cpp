// (c) Henrique Hissa 2020
#include "MCubes.h"

// Math Utilities
#include "Math/UnrealMathUtility.h" 
#include "GenericPlatform/GenericPlatformMath.h"

// Allow for lookup table that does not bloat compile time
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// HexFString to Int
#include "Misc/Parse.h"

// Normals and tangents calculations
#include "KismetProceduralMeshLibrary.h"

// Sets default values
AMCubes::AMCubes()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	// RootComponent = Mesh;

	// Mesh->bUseAsyncCooking = true;

	StaticProvider = NewObject<URuntimeMeshProviderStatic>(this, TEXT("RuntimeMeshProvider-Static"));
	if(StaticProvider) { GetRuntimeMeshComponent()->Initialize(StaticProvider); }
}

// Called when the game starts or when spawned
void AMCubes::BeginPlay()
{
	Super::BeginPlay();

	FakeVolume = ((MeshXDimension * MeshYDimension * MeshZDimension)/3)*MicroChunkResolution;

	LastSurfaceLevel = 0;

	// UE_LOG(LogClass, Warning, TEXT("Dimensions: %ix%ix%i"), MeshXDimension, MeshYDimension, MeshZDimension);

	// UE_LOG(LogClass, Warning, TEXT("TriTableCells TArray Length: %i // TriTable Length: %i"), TriTableCells.Num(), TriTable.Num());

	// Sets current seconds counter to 0
	Self = this;

	// const FVector DebugOffset = FVector((MeshXDimension/2)*MicroChunkResolution, (MeshYDimension/2)*MicroChunkResolution, (MeshZDimension/2)*MicroChunkResolution);

	// DrawDebugBox(AMCubes::GetWorld(), AMCubes::GetActorLocation() + DebugOffset, DebugOffset, FColor().Cyan, true, -1.f, (uint8)'\000', 5.f);

	MCTask = new FAutoDeleteAsyncTask<MarchingCubesAlgorithm>(Self/*, MeshTris, bBegunCalculations, bFinishedCalculations*/);
	MCTask->StartBackgroundTask();
}

// Called when the actor is being destroyed
void AMCubes::BeginDestroy()
{
	Super::BeginDestroy();

	// if(bBegunCalculations && !bFinishedCalculations) { MCTask->Cancel(); }
}

// Called every frame
void AMCubes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentSeconds += DeltaTime;

	if(CurrentSeconds >= 1)
	{
		if(FVector::Dist(RootParent->CamMgr->GetCameraLocation() + (*ChunkOffset * FakeVolume), GetActorLocation()) > (*RenderDistance + 3) * FakeVolume) 
		{
			ChunkList->Remove(GetActorLocation());
			Destroy();
			//SetActorHiddenInGame(true);
		}
		CurrentSeconds -= 1;
	}

	/*if((SurfaceLevel != LastSurfaceLevel || PNoise1 != LastPNoise) && !bBegunCalculations)
	{
		(new FAutoDeleteAsyncTask<MarchingCubesAlgorithm>(Self))->StartBackgroundTask();
		LastSurfaceLevel = SurfaceLevel;
		LastPNoise = PNoise1;
	}*/

	if(bFinishedCalculations)
	{
		if(MeshTris.Num() > 0)
		{
			// UE_LOG(LogClass, Warning, TEXT("Trying to create mesh!"));
			// Mesh->ClearAllMeshSections();
			// UE_LOG(LogClass, Warning, TEXT("Entering creation."));
			AMCubes::CreateTriangle();
			if((*ChunkList).Contains(GetActorLocation())) 
			{ 
				AActor* ChunkToDelete = (*ChunkList)[GetActorLocation()];
				ChunkList->Remove(GetActorLocation()); 
				ChunkToDelete->Destroy();
			}
			ChunkList->Add(GetActorLocation(), this);
		}
		else
		{
			// UE_LOG(LogClass, Error, TEXT("DELETING chunk %s"), *(GetActorLocation().ToCompactString()));
			/*ChunkList->Remove(GetActorLocation());
			Destroy();*/
			IgnorePositions->Add(GetActorLocation());
			Destroy();
		}
		MeshTris.Empty();
		bFinishedCalculations = bBegunCalculations = false;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Position: %s"), *(RootParent->CamMgr->GetCameraLocation().ToString()));
}

// Would be called once an actor spawns (either at runtime or when placed in the scene)
/*
void AMCubes::PostLoad()
{
	Super::PostLoad();
	// AMCubes::CreateTriangle(FVector(0, 0, 0), {FVector(0, 0, 0), FVector(0, MicroChunkResolution, 0), FVector(0, 0, MicroChunkResolution)});
}
*/

// Triangle creation
void AMCubes::CreateTriangle(/*const FVector& OriginPoint, const TArray<FVector>& VertexList*/)
{
	/*TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;*/
	TArray<FColor> VertexColors;

	/*for(const FVector& PointToTri : VertexList)
	{
		int VertexCount = Vertices.Num();
		Vertices.Add(OriginPoint + PointToTri - AMCubes::GetActorLocation());
		Triangles.Add(VertexCount++);
		// VertexColors.Add(FLinearColor(0.5, 0.5, 0.5, 1));

		// uint8 IndexMod = Index % 3;
		// UV0.Add(FVector2D((IndexMod == 1 ? 1 : 0), (IndexMod == 2 ? 1 : 0)));
		UV0.Add(FVector2D(PointToTri.X/MicroChunkResolution, PointToTri.Y/MicroChunkResolution));
	}*/


	// uint_t MeshNumSec = Mesh->GetNumSections();

	// AMCubes::CalculateTangentsForMeshCommented(Vertices, Triangles, UV0, Normals, Tangents);
	// Mesh->CreateMeshSection(0, MeshTris, MeshTriIndices, MeshNorms, MeshUVs, VertexColors, MeshTans, true);

	// Mesh->ContainsPhysicsTriMeshData(true);

	// Mesh->SetMaterial(0, MeshMaterial);

	StaticProvider->SetupMaterialSlot(0, TEXT("Mat"), MeshMaterial);

	// UE_LOG(LogClass, Warning, TEXT("Creating mesh section."));

	StaticProvider->CreateSectionFromComponents(0, 0, 0, MeshTris, MeshTriIndices, MeshNorms, MeshUVs, VertexColors, MeshTans, ERuntimeMeshUpdateFrequency::Infrequent, true);

	// UE_LOG(LogTemp, Warning, TEXT("Made mesh on section 0 with %i vertices"), MeshTris.Num());
}

// Makes mesh using marching cubes
void AMCubes::BeginMarch()
{
	// UE_LOG(LogClass, Warning, TEXT("MARCHING chunk %s"), *(GetActorLocation().ToCompactString()));
	// FlushPersistentDebugLines(AMCubes::GetWorld());

	const TArray<uint8> Increments = {1, 2, 8, 4, 16, 32, 128, 64};
	FOccluderVertexArray TriVerts, Norms;
	TArray<FProcMeshTangent> Tans;
	TArray<FVector2D> UVs;
	TArray<int> TriInds;

	uint32 MaxMarchesX = FGenericPlatformMath::CeilToInt(MeshXDimension*(100.0 / double(MicroChunkResolution)));
	uint32 MaxMarchesY = FGenericPlatformMath::CeilToInt(MeshYDimension*(100.0 / double(MicroChunkResolution)));
	uint32 MaxMarchesZ = FGenericPlatformMath::CeilToInt(MeshZDimension*(100.0 / double(MicroChunkResolution)));

	// Cycles through all Z planes
	for(uint32 ZIndex = 0; ZIndex < MaxMarchesZ; ZIndex++)
	{
		// Cycles through all Y axes
		for (uint32 YIndex = 0; YIndex < MaxMarchesY; YIndex++)
		{
			// Cycles through each X point to acces a voxel
			for (uint32 XIndex = 0; XIndex < MaxMarchesX; XIndex++)
			{
				FOccluderVertexArray MemberTriVerts/* , MemberNorms */;
				// TArray<FProcMeshTangent> MemberTans;
				TArray<FVector2D> MemberUVs;
				TArray<int> MemberTriInds;

				// FOccluderVertexArray TriVerts;
				// Store current vector coordinate
				FVector CurrentCoordinate = FVector(XIndex*MicroChunkResolution, YIndex*MicroChunkResolution, ZIndex*MicroChunkResolution);
				// if (bRenderDebug) { DrawDebugSphere(AMCubes::GetWorld(), AMCubes::GetActorLocation() + (VoxelVertexCoordinate * AMCubes::GetActorScale3D()), 25, 10, (MCPoints[VoxelVertexCoordinate] >= SurfaceLevel) ? FColor::Emerald : FColor::Black, true); }

				// UE_LOG(LogClass, Warning, TEXT("CurrentCoordinate = %s"), *CurrentCoordinate.ToString());

				// Create cube index variable
				uint8 CubeIndex = 0;

				// Store each point density in table to determine interpolation later
				TMap<FVector, float> PointDensityMap;

				// Cycles through each vertex in voxel
				for (uint8 VoxelIndex = 0; VoxelIndex < 8; VoxelIndex++)
				{
					FVector VoxelVertexCoordinate = CurrentCoordinate + FVector((VoxelIndex & 1)*MicroChunkResolution, ((VoxelIndex & 2) >> 1)*MicroChunkResolution, ((VoxelIndex & 4) >> 2)*MicroChunkResolution);

					// UE_LOG(LogClass, Warning, TEXT("%i"), VoxelIndex & 1);
					float DensityValue = AMCubes::DensityFunction(VoxelVertexCoordinate + AMCubes::GetActorLocation());
					PointDensityMap.Add(VoxelVertexCoordinate, DensityValue);

					if (DensityValue >= SurfaceLevel) { CubeIndex |= Increments[VoxelIndex]; }

					// UE_LOG(LogClass, Warning, TEXT("Voxel Vertex Coordinate: %s // Voxel Value: %f // Surface Level : %f"), *VoxelVertexCoordinate.ToString(), AMCubes::DensityFunction(VoxelVertexCoordinate), SurfaceLevel);
				}

				// UE_LOG(LogClass, Warning, TEXT("Cube Index: %i"), CubeIndex);

				// If mesh index is either 0 or 255 (completely out or in), skip everything
				if (CubeIndex == 0 || CubeIndex == 255) {continue;}

				const uint16 &CutEdges = (*EdgeTable)[CubeIndex];
				const TArray<int8> &TriTableValue = (*TriTable)[CubeIndex].List;

				// UE_LOG(LogClass, Warning, TEXT("Cube index for voxel: %i // EdgeTable's Value: %i // TriTable's first value for set index: %i"), CubeIndex, CutEdges, TriTableValue[0]);


				TArray<FVector> TriangleVertices;
				TriangleVertices.Init(FVector(0, 0, 0), 12);
				
				// Cycles through all cut edges
				for(uint8 EdgeIndex = 0; EdgeIndex < 12; EdgeIndex++)
				{
					// If edge byte is not equal to 1, edge was not cut, go to next edge
					if (!((CutEdges >> EdgeIndex) & 1)) { continue; }

					FVector P1 = CurrentCoordinate, P2 = CurrentCoordinate;

					// Avoids a switch case statement by assigning the specific values according to the edge index
					P1 += FVector((((EdgeIndex + 1) & 2) >> 1) * MicroChunkResolution, ((EdgeIndex & 2) >> 1) * MicroChunkResolution, ((EdgeIndex & 4) >> 2) * MicroChunkResolution);
					P2 += FVector(EdgeIndex < 8 ? (((EdgeIndex + 2) & 2) >> 1) * MicroChunkResolution : (((EdgeIndex - 7) & 2) >> 1) * MicroChunkResolution, EdgeIndex < 8 ? (((EdgeIndex + 1) & 2) >> 1) * MicroChunkResolution : (((EdgeIndex - 8) & 2) >> 1) * MicroChunkResolution, EdgeIndex < 8 ? ((EdgeIndex & 4) >> 2) * MicroChunkResolution : MicroChunkResolution);

					// TriangleVertices[EdgeIndex] = AMCubes::GetInterpolatedPosition(P1, P2, AMCubes::DensityFunction(P1), AMCubes::DensityFunction(P2));
					TriangleVertices[EdgeIndex] = AMCubes::GetInterpolatedPosition(P1, P2, PointDensityMap[P1], PointDensityMap[P2]);

					// UE_LOG(LogClass, Warning, TEXT("On edge %i, P1 = %s and P2 = %s"), EdgeIndex, *P1.ToString(), *P2.ToString());
					// UE_LOG(LogClass, Warning, TEXT("Edge %i is cut. Interpolated vertex is %s"), EdgeIndex, *(TriangleVertices[EdgeIndex].ToString()));
				}

				// Add each triangle vertex to list
				for(uint8 TriangleIndex = 0; TriTableValue[TriangleIndex] != -1; TriangleIndex += 3)
				{
					// FOccluderVertexArray MemberTriVerts, MemberNorms;
					// TArray<FProcMeshTangent> MemberTans;
					// TArray<FVector2D> MemberUVs;
					// TArray<int> MemberTriInds;

					int VertNum = TriVerts.Num() + MemberTriInds.Num();
					// FOccluderVertexArray TriVerts;
					MemberTriInds.Add(VertNum++);
					MemberTriVerts.Add(TriangleVertices[TriTableValue[TriangleIndex]]/*  - AMCubes::GetActorLocation() */);
					MemberUVs.Add(FVector2D(TriangleVertices[TriTableValue[TriangleIndex]].X/MicroChunkResolution, TriangleVertices[TriTableValue[TriangleIndex]].Y/MicroChunkResolution));

					MemberTriInds.Add(VertNum++);
					MemberTriVerts.Add(TriangleVertices[TriTableValue[TriangleIndex + 1]]/*  - AMCubes::GetActorLocation() */);
					MemberUVs.Add(FVector2D(TriangleVertices[TriTableValue[TriangleIndex + 1]].X/MicroChunkResolution, TriangleVertices[TriTableValue[TriangleIndex + 1]].Y/MicroChunkResolution));

					MemberTriInds.Add(VertNum++);
					MemberTriVerts.Add(TriangleVertices[TriTableValue[TriangleIndex + 2]]/*  - AMCubes::GetActorLocation() */);
					MemberUVs.Add(FVector2D(TriangleVertices[TriTableValue[TriangleIndex + 2]].X/MicroChunkResolution, TriangleVertices[TriTableValue[TriangleIndex + 2]].Y/MicroChunkResolution));
					// AMCubes::CreateTriangle(FVector(0, 0, 0), TriVerts);
					// AMCubes::CalculateTangentsForMeshCommented(MemberTriVerts, MemberTriInds, MemberUVs, MemberNorms, MemberTans);
					// TriVerts += MemberTriVerts;
					// TriInds += MemberTriInds;
					// UVs += MemberUVs;
					// Norms += MemberNorms;
					// Tans += MemberTans;
				}
				// AMCubes::CreateTriangle(FVector(0, 0, 0), TriVerts);
				// AMCubes::CalculateTangentsForMeshCommented(MemberTriVerts, MemberTriInds, MemberUVs, MemberNorms, MemberTans);

				TriVerts += MemberTriVerts;
				TriInds += MemberTriInds;
				UVs += MemberUVs;
				// Norms += MemberNorms;
				// Tans += MemberTans;

				// UE_LOG(LogClass, Warning, TEXT("First few triangles: %i, %i, %i (Actual length: %i)"), MemberTriInds[0], MemberTriInds[1], MemberTriInds[2], TriInds.Num());
			}
			
		}
		
	}

	// UE_LOG(LogClass, Warning, TEXT("100.00%% complete! %i/%i marches done."), MarchesToMake, MarchesToMake);

	// UE_LOG(LogClass, Warning, TEXT("Vertices: %i"), TriVerts.Num());

	/*UE_LOG(LogClass, Warning, TEXT("Calculating tangents."));
	AMCubes::CalculateTangentsForMeshCommented(TriVerts, TriInds, UVs, Norms, Tans);*/
	MeshTris = TriVerts;
	MeshTriIndices = TriInds;
	MeshUVs = UVs;
	// MeshNorms = Norms;
	// MeshTans = Tans;
	// Build mesh if it has triangles to build
	//if (TriVerts.Num() == 0) { return; }
	//AMCubes::CreateTriangle(FVector(0, 0, 0), TriVerts);
	// UE_LOG(LogClass, Warning, TEXT("Mesh sections: %i"), Mesh->GetNumSections());
	// UE_LOG(LogClass, Warning, TEXT("Vert Number: %i"), TriVerts.Num());
}

FVector AMCubes::GetInterpolatedPosition(const FVector& P1, const FVector& P2, const float& V1, const float& V2) const
{
	return P1 + ((SurfaceLevel - V1)*(P2 - P1))/(V2-V1);
}

float AMCubes::DensityFunction(const FVector& Point) const
{
	const FVector PointBroken3D = Point / FVector(101,103,107);
	const FVector2D PointBroken = FVector2D(Point.X / 101, Point.Y / 103);

	// float Density = (-Point.Z / MicroChunkResolution); // Creates flat ground;
	float Density = 0;

	for(int Index = 1; Index < 8; Index++)
	{
		if(RootParent->Frequencies.Num() > Index && RootParent->Amplitudes.Num() > Index)
		{
			Density += FMath::PerlinNoise2D(PointBroken * RootParent->Frequencies[Index]) * RootParent->Amplitudes[Index];
		}
		else
		{
			Density += FMath::PerlinNoise2D(PointBroken * FMath::Pow(2, 2-Index)) *  FMath::Pow(2, Index - 3);
		}
	}

	Density += FMath::PerlinNoise2D(FVector2D(FMath::PerlinNoise1D(PointBroken.X), FMath::PerlinNoise1D(PointBroken.Y))* RootParent->Frequencies[8]) * RootParent->Amplitudes[8];
	
	double WalkingPerlinNoise = FMath::PerlinNoise2D(PointBroken * RootParent->Frequencies[10]) * RootParent->Amplitudes[10];
	
	double WPNLevel = WalkingPerlinNoise < (*MinimumCutoff) ? (*MinimumCutoff) : (WalkingPerlinNoise > (*MaximumCutoff) ? (*MaximumCutoff) : WalkingPerlinNoise);
	
	double WalkingWeight = FMath::Pow(((WPNLevel - (*MinimumCutoff))/((*MaximumCutoff) - (*MinimumCutoff))), (*CutoffPower));


	Density *= WalkingWeight;

	// if(FMath::RandRange(0,10000) == 5) { UE_LOG(LogClass, Warning, TEXT("WW Value: %f"), WalkingWeight) }

	Density += (1 - WalkingWeight) * (*WalkHeight);

	// Density += (*BaseHeight);
	Density -= (WalkingWeight + ((1 - WalkingWeight)*(*OverhangPresence))) * FMath::PerlinNoise3D(PointBroken3D * RootParent->Frequencies[9]) * RootParent->Amplitudes[9]; // 0.0078127f) * 128.0f;

	Density += FMath::PerlinNoise2D(PointBroken * RootParent->Frequencies[0]) * RootParent->Amplitudes[0];


	
	// float PlateauValue = (*PlateauIntensity) * ((*PlateauHeight) * FMath::Floor(double((*PlateauTotalHeight) * FMath::PerlinNoise2D(PointBroken * RootParent->Frequencies[9]))/double(*PlateauHeight)));

	Density += ((*SedimentWeight) * FMath::PerlinNoise1D(PointBroken3D.Z * (*SedimentFrequency)));
	// Density /= PlateauValue < (*PlateauBias) ? 1 : PlateauValue;
	
	// Density += PlateauValue;

	Density -= Point.Z / 100; // Creates flat ground;
	// Density *= ((*SedimentWeight) * FMath::PerlinNoise1D(PointBroken3D.Z * (*SedimentFrequency))) + 1;


	// Density -= FMath::PerlinNoise3D(PointBroken3D * 0.062476) * 16; // 0.0078127f) * 128.0f;
	// Density += (cos(Point.X / MicroChunkResolution0) * sin(Point.Y / MicroChunkResolution0))*10;
	// Density += FMath::PerlinNoise2D(PointBroken * 0.06261f) * 16.0f;
	// Density += FMath::PerlinNoise2D(PointBroken * 0.03124f) * 32.0f;
	// Density += FMath::PerlinNoise2D(PointBroken * 0.015623f) * 64.0f;
	// (-Point.Z / MicroChunkResolution) + PNoise1 * FMath::PerlinNoise3D()
	return Density;
}

MarchingCubesAlgorithm::MarchingCubesAlgorithm(AMCubes* ActorMeshIn)
{
	ActorMesh = ActorMeshIn;
}
 
MarchingCubesAlgorithm::~MarchingCubesAlgorithm()
{
	// UE_LOG(LogClass, Warning, TEXT("Leaving external thread."));
	ActorMesh->bFinishedCalculations = true;
	// UE_LOG(LogClass, Warning, TEXT("Finished march!"));
}

void MarchingCubesAlgorithm::DoWork()
{
	ActorMesh->bBegunCalculations = true;
	// UE_LOG(LogClass, Warning, TEXT("Begun march!"));
	ActorMesh->BeginMarch();
}

/*************************************************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON ORIGINAL CONTENT AHEAD!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*************************************************************************************************************************************/

void AMCubes::CalculateTangentsForMeshCommented(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents)
{
	// SCOPE_CYCLE_COUNTER(STAT_ProcMesh_CalcTangents);

	if (Vertices.Num() == 0)
	{
		return;
	}

	// Number of triangles
	const int32 NumTris = Triangles.Num() / 3;
	// Number of verts
	const int32 NumVerts = Vertices.Num();

	// Map of vertex to triangles in Triangles array
	TMultiMap<int32, int32> VertToTriMap;
	// Map of vertex to triangles to consider for normal calculation
	TMultiMap<int32, int32> VertToTriSmoothMap;

	// Normal/tangents for each face
	TArray<FVector> FaceTangentX, FaceTangentY, FaceTangentZ;
	FaceTangentX.AddUninitialized(NumTris);
	FaceTangentY.AddUninitialized(NumTris);
	FaceTangentZ.AddUninitialized(NumTris);

	double Percentage = 0;
	double PercentageDone = 0;

	// Iterate over triangles
	for (int TriIdx = 0; TriIdx < NumTris; TriIdx++)
	{
		PercentageDone = 100*(double(TriIdx)/double(NumTris));

		if(PercentageDone > Percentage)
		{
			// UE_LOG(LogClass, Warning, TEXT("%.1f%% done iterating over triangles // %i/%i (Stage 1/3 of tangents)"), PercentageDone, TriIdx, NumTris);
			Percentage += 0.5;
		}

		int32 CornerIndex[3];
		FVector P[3];

		for (int32 CornerIdx = 0; CornerIdx < 3; CornerIdx++)
		{
			// Find vert index (clamped within range)
			int32 VertIndex = FMath::Min(Triangles[(TriIdx * 3) + CornerIdx], NumVerts - 1);

			CornerIndex[CornerIdx] = VertIndex;
			P[CornerIdx] = Vertices[VertIndex];

			// Find/add this vert to index buffer
			TArray<int32> VertOverlaps;
			AMCubes::FindVertOverlaps(VertIndex, Vertices, VertOverlaps);

			// Remember which triangles map to this vert
			VertToTriMap.AddUnique(VertIndex, TriIdx);
			VertToTriSmoothMap.AddUnique(VertIndex, TriIdx);

			// Also update map of triangles that 'overlap' this vert (ie don't match UV, but do match smoothing) and should be considered when calculating normal
			for (int32 OverlapIdx = 0; OverlapIdx < VertOverlaps.Num(); OverlapIdx++)
			{
				// For each vert we overlap..
				int32 OverlapVertIdx = VertOverlaps[OverlapIdx];

				// Add this triangle to that vert
				VertToTriSmoothMap.AddUnique(OverlapVertIdx, TriIdx);

				// And add all of its triangles to us
				TArray<int32> OverlapTris;
				VertToTriMap.MultiFind(OverlapVertIdx, OverlapTris);
				for (int32 OverlapTriIdx = 0; OverlapTriIdx < OverlapTris.Num(); OverlapTriIdx++)
				{
					VertToTriSmoothMap.AddUnique(VertIndex, OverlapTris[OverlapTriIdx]);
				}
			}
		}

		// Calculate triangle edge vectors and normal
		const FVector Edge21 = P[1] - P[2];
		const FVector Edge20 = P[0] - P[2];
		const FVector TriNormal = (Edge21 ^ Edge20).GetSafeNormal();

		// If we have UVs, use those to calc 
		if (UVs.Num() == Vertices.Num())
		{
			const FVector2D T1 = UVs[CornerIndex[0]];
			const FVector2D T2 = UVs[CornerIndex[1]];
			const FVector2D T3 = UVs[CornerIndex[2]];

			FMatrix	ParameterToLocal(
				FPlane(P[1].X - P[0].X, P[1].Y - P[0].Y, P[1].Z - P[0].Z, 0),
				FPlane(P[2].X - P[0].X, P[2].Y - P[0].Y, P[2].Z - P[0].Z, 0),
				FPlane(P[0].X, P[0].Y, P[0].Z, 0),
				FPlane(0, 0, 0, 1)
				);

			FMatrix ParameterToTexture(
				FPlane(T2.X - T1.X, T2.Y - T1.Y, 0, 0),
				FPlane(T3.X - T1.X, T3.Y - T1.Y, 0, 0),
				FPlane(T1.X, T1.Y, 1, 0),
				FPlane(0, 0, 0, 1)
				);

			// Use InverseSlow to catch singular matrices.  Inverse can miss this sometimes.
			const FMatrix TextureToLocal = ParameterToTexture.Inverse() * ParameterToLocal;

			FaceTangentX[TriIdx] = TextureToLocal.TransformVector(FVector(1, 0, 0)).GetSafeNormal();
			FaceTangentY[TriIdx] = TextureToLocal.TransformVector(FVector(0, 1, 0)).GetSafeNormal();
		}
		else
		{
			FaceTangentX[TriIdx] = Edge20.GetSafeNormal();
			FaceTangentY[TriIdx] = (FaceTangentX[TriIdx] ^ TriNormal).GetSafeNormal();
		}

		FaceTangentZ[TriIdx] = TriNormal;
	}


	// Arrays to accumulate tangents into
	TArray<FVector> VertexTangentXSum, VertexTangentYSum, VertexTangentZSum;
	VertexTangentXSum.AddZeroed(NumVerts);
	VertexTangentYSum.AddZeroed(NumVerts);
	VertexTangentZSum.AddZeroed(NumVerts);

	Percentage = PercentageDone = 0;

	// For each vertex..
	for (int VertxIdx = 0; VertxIdx < Vertices.Num(); VertxIdx++)
	{
		PercentageDone = 100*(double(VertxIdx)/double(Vertices.Num()));

		if(PercentageDone > Percentage)
		{
			// UE_LOG(LogClass, Warning, TEXT("%.1f%% done iterating over vertices // %i/%i (Stage 2/3 of tangents)"), PercentageDone, VertxIdx, Vertices.Num());
			Percentage += 0.5;
		}

		// Find relevant triangles for normal
		TArray<int32> SmoothTris;
		VertToTriSmoothMap.MultiFind(VertxIdx, SmoothTris);

		for (int i = 0; i < SmoothTris.Num(); i++)
		{
			int32 TriIdx = SmoothTris[i];
			VertexTangentZSum[VertxIdx] += FaceTangentZ[TriIdx];
		}

		// Find relevant triangles for tangents
		TArray<int32> TangentTris;
		VertToTriMap.MultiFind(VertxIdx, TangentTris);

		for (int i = 0; i < TangentTris.Num(); i++)
		{
			int32 TriIdx = TangentTris[i];
			VertexTangentXSum[VertxIdx] += FaceTangentX[TriIdx];
			VertexTangentYSum[VertxIdx] += FaceTangentY[TriIdx];
		}
	}

	// Finally, normalize tangents and build output arrays

	Normals.Reset();
	Normals.AddUninitialized(NumVerts);

	Tangents.Reset();
	Tangents.AddUninitialized(NumVerts);

	Percentage = PercentageDone = 0;

	for (int VertxIdx = 0; VertxIdx < NumVerts; VertxIdx++)
	{
		PercentageDone = 100*(double(VertxIdx)/double(NumVerts));

		if(PercentageDone > Percentage)
		{
			// UE_LOG(LogClass, Warning, TEXT("%.1f%% done normalizing tangents // %i/%i (Stage 3/3 of tangents)"), PercentageDone, VertxIdx, NumVerts);
			Percentage += 0.5;
		}

		FVector& TangentX = VertexTangentXSum[VertxIdx];
		FVector& TangentY = VertexTangentYSum[VertxIdx];
		FVector& TangentZ = VertexTangentZSum[VertxIdx];

		TangentX.Normalize();
		TangentZ.Normalize();

		Normals[VertxIdx] = TangentZ;

		// Use Gram-Schmidt orthogonalization to make sure X is orth with Z
		TangentX -= TangentZ * (TangentZ | TangentX);
		TangentX.Normalize();

		// See if we need to flip TangentY when generating from cross product
		const bool bFlipBitangent = ((TangentZ ^ TangentX) | TangentY) < 0.f;

		Tangents[VertxIdx] = FProcMeshTangent(TangentX, bFlipBitangent);
	}
}

void AMCubes::FindVertOverlaps(int32 TestVertIndex, const TArray<FVector>& Verts, TArray<int32>& VertOverlaps)
{
	// Check if Verts is empty or test is outside range
	if (TestVertIndex < Verts.Num())
	{
		const FVector TestVert = Verts[TestVertIndex];

		for (int32 VertIdx = 0; VertIdx < Verts.Num(); VertIdx++)
		{
			// First see if we overlap, and smoothing groups are the same
			if (TestVert.Equals(Verts[VertIdx]))
			{
				// If it, so we are at least considered an 'overlap' for normal gen
				VertOverlaps.Add(VertIdx);
			}
		}
	}
}