// (c) Henrique Hissa 2020-2021
#include "MCubesIndependent.h"

// Math Utilities
#include "Math/UnrealMathUtility.h" 

// Allow for lookup table that does not bloat compile time
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// HexFString to Int
#include "Misc/Parse.h"

// Normals and tangents calculations
#include "KismetProceduralMeshLibrary.h"

// Sets default values
AMCubesIndependent::AMCubesIndependent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	// RootComponent = Mesh;

	// Mesh->bUseAsyncCooking = true;

	/*StaticProviderIndependent = NewObject<URuntimeMeshProviderStatic>(this, TEXT("RuntimeMeshProvider-Static"));
	if(StaticProviderIndependent) { AMCubesIndependent::GetRuntimeMeshComponent()->Initialize(StaticProviderIndependent); }*/
}

// Called when the game starts or when spawned
void AMCubesIndependent::BeginPlay()
{
	Super::BeginPlay();

	// Loads edge lookup table (ensures fast compile times)
	const FString EdgeTablePath = FPaths::ProjectContentDir() + TEXT("LookupTable/edgetable.txt");
	TArray<FString> EdgeTableCells;
	FFileHelper::LoadFileToStringArray(EdgeTableCells, *EdgeTablePath);

	for(const FString& ETCell : EdgeTableCells)
	{
		EdgeTable.Add(FParse::HexNumber(*ETCell));
	}

	// UE_LOG(LogClass, Warning, TEXT("EdgeTableCells TArray Length: %i // EdgeTable Length: %i"), EdgeTableCells.Num(), EdgeTable.Num());

	// Loads triangle lookup table
	const FString TriTablePath = FPaths::ProjectContentDir() + TEXT("LookupTable/tritable.txt");
	TArray<FString> TriTableCells;
	FFileHelper::LoadFileToStringArray(TriTableCells, *TriTablePath);

	for(const FString& TTCell : TriTableCells)
	{
		FLookupCellIndependent NewCell;
		TArray<FString> NumsRaw;

		TTCell.ParseIntoArray(NumsRaw, TEXT(","));

		for(const FString& Num : NumsRaw)
		{
			NewCell.List.Add(FCString::Atoi(*Num));
		}

		TriTable.Add(NewCell);
	}

	FakeVolume = ((MeshXDimension * MeshYDimension * MeshZDimension)/3)*100;

	LastSurfaceLevel = 0;

	UE_LOG(LogClass, Warning, TEXT("Dimensions: %ix%ix%i"), MeshXDimension, MeshYDimension, MeshZDimension);

	// UE_LOG(LogClass, Warning, TEXT("TriTableCells TArray Length: %i // TriTable Length: %i"), TriTableCells.Num(), TriTable.Num());

	// Sets current seconds counter to 0
	Self = this;

	MCTask = new FAutoDeleteAsyncTask<MarchingCubesAlgorithmIndependent>(Self/*, MeshTris, bBegunCalculations, bFinishedCalculations*/);
	MCTask->StartBackgroundTask();
}

// Called when the actor is being destroyed
void AMCubesIndependent::BeginDestroy()
{
	Super::BeginDestroy();

	// if(bBegunCalculations && !bFinishedCalculations) { MCTask->Cancel(); }
}

// Called every frame
void AMCubesIndependent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			// Mesh->ClearAllMeshSections();
			AMCubesIndependent::CreateTriangle();
			// ChunkList->Add(GetActorLocation(), this);
		}
		else
		{
			UE_LOG(LogClass, Error, TEXT("DELETING chunk %s"), *(GetActorLocation().ToCompactString()));
			Destroy();
		}
		MeshTris.Empty();
		bFinishedCalculations = bBegunCalculations = false;

		// PrimaryActorTick.bCanEverTick = false;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Position: %s"), *(RootParent->CamMgr->GetCameraLocation().ToString()));
}

// Would be called once an actor spawns (either at runtime or when placed in the scene)
/*
void AMCubesIndependent::PostLoad()
{
	Super::PostLoad();
	// AMCubesIndependent::CreateTriangle(FVector(0, 0, 0), {FVector(0, 0, 0), FVector(0, 100, 0), FVector(0, 0, 100)});
}
*/

// Triangle creation
void AMCubesIndependent::CreateTriangle(/*const FVector& OriginPoint, const TArray<FVector>& VertexList*/)
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
		Vertices.Add(OriginPoint + PointToTri - AMCubesIndependent::GetActorLocation());
		Triangles.Add(VertexCount++);
		// VertexColors.Add(FLinearColor(0.5, 0.5, 0.5, 1));

		// uint8 IndexMod = Index % 3;
		// UV0.Add(FVector2D((IndexMod == 1 ? 1 : 0), (IndexMod == 2 ? 1 : 0)));
		UV0.Add(FVector2D(PointToTri.X/100, PointToTri.Y/100));
	}*/


	// uint_t MeshNumSec = Mesh->GetNumSections();

	// UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);
	// Mesh->CreateMeshSection(0, MeshTris, MeshTriIndices, MeshNorms, MeshUVs, VertexColors, MeshTans, true);

	// Mesh->ContainsPhysicsTriMeshData(true);

	// Mesh->SetMaterial(0, MeshMaterial);

	/*if(StaticProviderIndependent) 
	// if(false) 
	{ 
		// GetRuntimeMeshComponent()->Initialize(StaticProviderIndependent); 
		StaticProviderIndependent->SetupMaterialSlot(0, TEXT("Mat"), MeshMaterial);
		StaticProviderIndependent->CreateSectionFromComponents(0, 0, 0, MeshTris, MeshTriIndices, MeshNorms, MeshUVs, VertexColors, MeshTans, ERuntimeMeshUpdateFrequency::Infrequent, true);	
	}*/


	// UE_LOG(LogTemp, Warning, TEXT("Made mesh on section 0 with %i vertices"), MeshTris.Num());
}

// Makes mesh using marching cubes
void AMCubesIndependent::BeginMarch()
{
	UE_LOG(LogClass, Warning, TEXT("MARCHING independent chunk %s"), *(GetActorLocation().ToCompactString()));
	// FlushPersistentDebugLines(AMCubesIndependent::GetWorld());

	const TArray<uint8> Increments = {1, 2, 8, 4, 16, 32, 128, 64};
	TArray<FVector> TriVerts, Norms;
	TArray<FProcMeshTangent> Tans;
	TArray<FVector2D> UVs;
	TArray<int> TriInds;

	// Cycles through all Z planes
	for(uint8 ZIndex = 0; ZIndex < MeshZDimension - 1; ZIndex++)
	{
		// Cycles through all Y axes
		for (uint8 YIndex = 0; YIndex < MeshYDimension - 1; YIndex++)
		{
			// Cycles through each X point to acces a voxel
			for (uint8 XIndex = 0; XIndex < MeshXDimension - 1; XIndex++)
			{
				TArray<FVector> MemberTriVerts/* , MemberNorms */;
				// TArray<FProcMeshTangent> MemberTans;
				TArray<FVector2D> MemberUVs;
				TArray<int> MemberTriInds;

				// TArray<FVector> TriVerts;
				// Store current vector coordinate
				FVector CurrentCoordinate = FVector(XIndex*100, YIndex*100, ZIndex*100);// + AMCubesIndependent::GetActorLocation();
				// if (bRenderDebug) { DrawDebugSphere(AMCubesIndependent::GetWorld(), AMCubesIndependent::GetActorLocation() + (VoxelVertexCoordinate * AMCubesIndependent::GetActorScale3D()), 25, 10, (MCPoints[VoxelVertexCoordinate] >= SurfaceLevel) ? FColor::Emerald : FColor::Black, true); }

				UE_LOG(LogClass, Warning, TEXT("CurrentCoordinate = %s"), *CurrentCoordinate.ToString());

				// Create cube index variable
				uint8 CubeIndex = 0;

				// Store each point density in table to determine interpolation later
				TMap<FVector, float> PointDensityMap;

				// Cycles through each vertex in voxel
				for (uint8 VoxelIndex = 0; VoxelIndex < 8; VoxelIndex++)
				{
					FVector VoxelVertexCoordinate = CurrentCoordinate + FVector((VoxelIndex & 1)*100, ((VoxelIndex & 2) >> 1)*100, ((VoxelIndex & 4) >> 2)*100);

					UE_LOG(LogClass, Warning, TEXT("%i"), VoxelIndex & 1);
					float DensityValue = AMCubesIndependent::DensityFunction(VoxelVertexCoordinate + AMCubesIndependent::GetActorLocation());
					PointDensityMap.Add(VoxelVertexCoordinate, DensityValue);

					if (DensityValue >= SurfaceLevel) { CubeIndex |= Increments[VoxelIndex]; }

					UE_LOG(LogClass, Warning, TEXT("Voxel Vertex Coordinate: %s // Voxel Value: %f // Surface Level : %f"), *VoxelVertexCoordinate.ToString(), AMCubesIndependent::DensityFunction(VoxelVertexCoordinate), SurfaceLevel);
				}

				// If mesh index is either 0 or 255 (completely out or in), skip everything
				if (CubeIndex == 0 || CubeIndex == 255) {continue;}

				const uint16 &CutEdges = EdgeTable[CubeIndex];
				const TArray<int8> &TriTableValue = TriTable[CubeIndex].List;

				UE_LOG(LogClass, Warning, TEXT("Cube index for voxel: %i // EdgeTable's Value: %i // TriTable's first value for set index: %i"), CubeIndex, CutEdges, TriTableValue[0]);


				TArray<FVector> TriangleVertices;
				TriangleVertices.Init(FVector(0, 0, 0), 12);
				
				// Cycles through all cut edges
				for(uint8 EdgeIndex = 0; EdgeIndex < 12; EdgeIndex++)
				{
					// If edge byte is not equal to 1, edge was not cut, go to next edge
					if (!((CutEdges >> EdgeIndex) & 1)) { continue; }

					FVector P1 = CurrentCoordinate, P2 = CurrentCoordinate;

					// Avoids a switch case statement by assigning the specific values according to the edge index
					P1 += FVector((((EdgeIndex + 1) & 2) >> 1) * 100, ((EdgeIndex & 2) >> 1) * 100, ((EdgeIndex & 4) >> 2) * 100);
					P2 += FVector(EdgeIndex < 8 ? (((EdgeIndex + 2) & 2) >> 1) * 100 : (((EdgeIndex - 7) & 2) >> 1) * 100, EdgeIndex < 8 ? (((EdgeIndex + 1) & 2) >> 1) * 100 : (((EdgeIndex - 8) & 2) >> 1) * 100, EdgeIndex < 8 ? ((EdgeIndex & 4) >> 2) * 100 : 100);

					// TriangleVertices[EdgeIndex] = AMCubesIndependent::GetInterpolatedPosition(P1, P2, AMCubesIndependent::DensityFunction(P1), AMCubesIndependent::DensityFunction(P2));
					TriangleVertices[EdgeIndex] = AMCubesIndependent::GetInterpolatedPosition(P1, P2, PointDensityMap[P1], PointDensityMap[P2]);

					// UE_LOG(LogClass, Warning, TEXT("On edge %i, P1 = %s and P2 = %s"), EdgeIndex, *P1.ToString(), *P2.ToString());
					UE_LOG(LogClass, Warning, TEXT("Edge %i is cut. Interpolated vertex is %s"), EdgeIndex, *(TriangleVertices[EdgeIndex].ToString()));
				}

				// Add each triangle vertex to list
				for(uint8 TriangleIndex = 0; TriTableValue[TriangleIndex] != -1; TriangleIndex += 3)
				{
					// TArray<FVector> MemberTriVerts, MemberNorms;
					// TArray<FProcMeshTangent> MemberTans;
					// TArray<FVector2D> MemberUVs;
					// TArray<int> MemberTriInds;

					int VertNum = TriVerts.Num() + MemberTriInds.Num();
					// TArray<FVector> TriVerts;
					MemberTriInds.Add(VertNum++);
					MemberTriVerts.Add(TriangleVertices[TriTableValue[TriangleIndex]]/*  - AMCubesIndependent::GetActorLocation() */);
					MemberUVs.Add(FVector2D(TriangleVertices[TriTableValue[TriangleIndex]].X/100, TriangleVertices[TriTableValue[TriangleIndex]].Y/100));

					MemberTriInds.Add(VertNum++);
					MemberTriVerts.Add(TriangleVertices[TriTableValue[TriangleIndex + 1]]/*  - AMCubesIndependent::GetActorLocation() */);
					MemberUVs.Add(FVector2D(TriangleVertices[TriTableValue[TriangleIndex + 1]].X/100, TriangleVertices[TriTableValue[TriangleIndex + 1]].Y/100));

					MemberTriInds.Add(VertNum++);
					MemberTriVerts.Add(TriangleVertices[TriTableValue[TriangleIndex + 2]]/*  - AMCubesIndependent::GetActorLocation() */);
					MemberUVs.Add(FVector2D(TriangleVertices[TriTableValue[TriangleIndex + 2]].X/100, TriangleVertices[TriTableValue[TriangleIndex + 2]].Y/100));
					// AMCubesIndependent::CreateTriangle(FVector(0, 0, 0), TriVerts);
					// UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MemberTriVerts, MemberTriInds, MemberUVs, MemberNorms, MemberTans);
					// TriVerts += MemberTriVerts;
					// TriInds += MemberTriInds;
					// UVs += MemberUVs;
					// Norms += MemberNorms;
					// Tans += MemberTans;
				}
				// AMCubesIndependent::CreateTriangle(FVector(0, 0, 0), TriVerts);
				// UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MemberTriVerts, MemberTriInds, MemberUVs, MemberNorms, MemberTans);

				TriVerts += MemberTriVerts;
				TriInds += MemberTriInds;
				UVs += MemberUVs;
				// Norms += MemberNorms;
				// Tans += MemberTans;

				UE_LOG(LogClass, Warning, TEXT("First few triangles: %i, %i, %i (Actual length: %i)"), MemberTriInds[0], MemberTriInds[1], MemberTriInds[2], TriInds.Num());
			}
			
		}
		
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(TriVerts, TriInds, UVs, Norms, Tans);
	MeshTris = TriVerts;
	MeshTriIndices = TriInds;
	MeshUVs = UVs;
	// MeshNorms = Norms;
	// MeshTans = Tans;
	// Build mesh if it has triangles to build
	//if (TriVerts.Num() == 0) { return; }
	//AMCubesIndependent::CreateTriangle(FVector(0, 0, 0), TriVerts);
	// UE_LOG(LogClass, Warning, TEXT("Mesh sections: %i"), Mesh->GetNumSections());
	// UE_LOG(LogClass, Warning, TEXT("Vert Number: %i"), TriVerts.Num());
}

FVector AMCubesIndependent::GetInterpolatedPosition(const FVector& P1, const FVector& P2, const float& V1, const float& V2) const
{
	return P1 + ((SurfaceLevel - V1)*(P2 - P1))/(V2-V1);
}

float AMCubesIndependent::DensityFunction(const FVector& Point) const
{
	const FVector PointBroken3D = Point / FVector(101,103,107);
	const FVector2D PointBroken = FVector2D(Point.X / 101, Point.Y / 103);

	float Density = (-Point.Z / 100); // Creates flat ground;

	/*Density += FMath::PerlinNoise2D(PointBroken   * RootParent->Frequencies[0]) * RootParent->Amplitudes[0]; // First octave (sample) of noise
	Density += FMath::PerlinNoise2D(PointBroken   * RootParent->Frequencies[1]) * RootParent->Amplitudes[1]; // Second octave (sample) of noise. It's important to double amplitude but reduce frequency by a value CLOSE to half, but not exactly, to avoid repetition
	Density += FMath::PerlinNoise2D(PointBroken   * RootParent->Frequencies[2]) * RootParent->Amplitudes[2]; // Third octave (sample) of noise. It's recommended to go up to nine octaves, but I don't think UE will run fast with nine values.
	Density += FMath::PerlinNoise2D(PointBroken   * RootParent->Frequencies[3]) * RootParent->Amplitudes[3];
	Density += FMath::PerlinNoise2D(PointBroken   * RootParent->Frequencies[4]) * RootParent->Amplitudes[4];
	Density += FMath::PerlinNoise2D(PointBroken   * RootParent->Frequencies[5]) * RootParent->Amplitudes[5];
	Density -= FMath::PerlinNoise3D(PointBroken3D * RootParent->Frequencies[6]) * RootParent->Amplitudes[6]; // 0.0078127f) * 128.0f;*/
	Density += (cos(Point.X / 1000) * sin(Point.Y / 1000))*10;
	// Density += FMath::PerlinNoise2D(PointBroken * 0.06261f) * 16.0f;
	// Density += FMath::PerlinNoise2D(PointBroken * 0.03124f) * 32.0f;
	// Density += FMath::PerlinNoise2D(PointBroken * 0.015623f) * 64.0f;
	// (-Point.Z / 100) + PNoise1 * FMath::PerlinNoise3D()
	return Density;
}

MarchingCubesAlgorithmIndependent::MarchingCubesAlgorithmIndependent(AMCubesIndependent* ActorMeshIn)
{
	ActorMesh = ActorMeshIn;
}
 
MarchingCubesAlgorithmIndependent::~MarchingCubesAlgorithmIndependent()
{
	ActorMesh->bFinishedCalculations = true;
	UE_LOG(LogClass, Warning, TEXT("Finished march!"));
}

void MarchingCubesAlgorithmIndependent::DoWork()
{
	ActorMesh->bBegunCalculations = true;
	UE_LOG(LogClass, Warning, TEXT("Begun march!"));
	ActorMesh->BeginMarch();
}

