// (c) Henrique Hissa 2020
#include "MCParent.h"

// Allow for lookup table that does not bloat compile time
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Grabs camera manager
#include "Kismet/GameplayStatics.h"

// Allows for creation of MCubes actors
#include "MCubes.h"

// Sets default values
AMCParent::AMCParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMCParent::BeginPlay()
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
		FLookupCell NewCell;
		TArray<FString> NumsRaw;

		TTCell.ParseIntoArray(NumsRaw, TEXT(","));

		for(const FString& Num : NumsRaw)
		{
			NewCell.List.Add(FCString::Atoi(*Num));
		}

		TriTable.Add(NewCell);
	}
	CamMgr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	bFirstFrame = true;
}

// Called every frame
void AMCParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector CamPosition = CamMgr->GetCameraLocation();
	bArraysChanged = (ArrayChanged(LastFrequencies, Frequencies) || ArrayChanged(LastAmplitudes, Amplitudes));
	CameraChunkPosition = FVector(FMath::FloorToInt(CamPosition.X / (ChunkSize * MicroChunkResolution)) * (ChunkSize * MicroChunkResolution), FMath::FloorToInt(CamPosition.Y / (ChunkSize * MicroChunkResolution)) * (ChunkSize * MicroChunkResolution), FMath::FloorToInt(CamPosition.Z / (ChunkSize * MicroChunkResolution)) * (ChunkSize * MicroChunkResolution));
	if((CameraLastChunkPosition != CameraChunkPosition || bArraysChanged || bFirstFrame) && ((!bAlreadyCreated && bCreateOnlyOne) || !bCreateOnlyOne))
	{
		for(int8 ReadChunkZ = -RenderDistance; ReadChunkZ <= RenderDistance; ReadChunkZ++)
		{
			if(bLimitRenderHeight && (ReadChunkZ < RenderHeightMin || ReadChunkZ > RenderHeightMax) && !bCreateOnlyOne)
			{
				continue;
			}
			for(int8 ReadChunkY = -RenderDistance; ReadChunkY <= RenderDistance; ReadChunkY++)
			{
				for(int8 ReadChunkX = -RenderDistance; ReadChunkX <= RenderDistance; ReadChunkX++)
				{
					// UE_LOG(LogClass, Warning, TEXT("Current position: %s // Chunk Location: %s"), *CamPosition.ToString(), *ChunkLocation.ToString());
					FVector NewChunkPos = FVector(ReadChunkX*(ChunkSize * MicroChunkResolution), ReadChunkY*(ChunkSize * MicroChunkResolution), ReadChunkZ*(ChunkSize * MicroChunkResolution)) + CameraChunkPosition + (ChunkSpawnOffset * (ChunkSize * MicroChunkResolution));
					
					if(bCreateOnlyOne) 
					{ 
						NewChunkPos = SingleChunkSpawnLocation;
						UE_LOG(LogClass, Warning, TEXT("Chunk Pos: %s (%.2f // %.1f)"), *(NewChunkPos.ToString()), CamPosition.Z, AMCParent::GetWorld()->TimeSeconds);
					}
					
					// UE_LOG(LogClass, Warning, TEXT("New Chunk Pos: %s\nCam Chunk Pos: %s"), *(NewChunkPos.ToCompactString()), *(CameraChunkPosition.ToCompactString()));
					if(!OccupiedPositions.Contains(NewChunkPos))
					{
						if(!bCreateOnlyOne) { OccupiedPositions.Add(NewChunkPos); }
						AMCParent::SpawnMesh(NewChunkPos);
					}
				}	
			}
		}
		CameraLastChunkPosition = CameraChunkPosition;
		LastFrequencies = Frequencies;
		LastAmplitudes = Amplitudes;
	}

	if(bFirstFrame) { bFirstFrame = false; }
}

void AMCParent::SpawnMesh(const FVector& Location)
{
	UE_LOG(LogClass, Warning, TEXT("Spawning Mesh."));
	if(!bAlreadyCreated) { bAlreadyCreated = true; }
	/*if(Chunks.Contains(Location)) 
	{
		if(bArraysChanged)
		{ 
			Chunks[Location]->Destroy();
			Chunks.Remove(Location);
			UE_LOG(LogClass, Warning, TEXT("Changed frequency or amplitude!"));
		}
		// UE_LOG(LogClass, Warning, TEXT("Chunk %s already exists!"), *Location.ToString());
		else
		{
			Chunks[Location]->SetActorHiddenInGame(false);
		}
	}
	else*/
	{
		AMCubes* CubeMesh = AMCParent::GetWorld()->SpawnActorDeferred<AMCubes>(AMCubes::StaticClass(), FTransform(Location), this);
		CubeMesh->RootParent = this;
		CubeMesh->ChunkList = &Chunks;
		CubeMesh->TriTable = &TriTable;
		CubeMesh->EdgeTable = &EdgeTable;
		CubeMesh->PlateauBias = &PlateauBias;
		CubeMesh->MeshMaterial = MeshMaterial;
		CubeMesh->PlateauHeight = &PlateauHeight;
		CubeMesh->MeshXDimension = ChunkSize + 1;
		CubeMesh->MeshYDimension = ChunkSize + 1;
		CubeMesh->MeshZDimension = ChunkSize + 1;
		CubeMesh->ChunkOffset = &ChunkSpawnOffset;
		CubeMesh->RenderDistance = &RenderDistance;
		CubeMesh->PlateauIntensity = &PlateauIntensity;
		CubeMesh->SedimentaryWeight = &SedimentaryWeight;
		CubeMesh->PlateauTotalHeight = &PlateauTotalHeight;
		CubeMesh->SedimentaryFrequency = &SedimentaryFrequency;
		CubeMesh->MicroChunkResolution = &MicroChunkResolution;
		CubeMesh->FinishSpawning(FTransform(Location));
		// CubeMesh->SetOwner(this);
		// Chunks.Add(Location, CubeMesh);
	}
}

bool AMCParent::ArrayChanged(const TArray<float>& ArrayA, const TArray<float>& ArrayB) const
{
	if(ArrayA.Num() != ArrayB.Num()) { return false; }

	for(uint8 Index = 0; Index < ArrayA.Num(); Index++)
	{
		if(ArrayA[Index] != ArrayB[Index]) { return true; }
	}

	return false;
}