// (c) Henrique Hissa 2020
#include "MCParent.h"

// Allow for lookup table that does not bloat compile time
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Math Utilities Again
#include "Math/UnrealMathUtility.h"

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
		int8 XValue = 0, YValue = 0, ZValue = 0;
		uint8 LoopState = 0, MaxVal = 1;
		TArray<FVector> VisitedChunks;
		bool bFinishedCircling = false;


		while(!bFinishedCircling)
		{
			// Save chunk coordinate to variable

			FVector ChunkCoord = FVector(XValue, YValue, ZValue);

			// Is this chunk valid?

			if(!(bLimitRenderHeight && (ZValue < RenderHeightMin || ZValue > RenderHeightMax)) && !IgnorePositions.Contains(ChunkCoord))
			{
				// It is! So, generate chunk

				FVector NewChunkPos = FVector(XValue*ChunkSize*100, YValue*ChunkSize*100, ZValue*ChunkSize*100) + CameraChunkPosition + (ChunkSpawnOffset * ChunkSize*100);
					
				if(bCreateOnlyOne) 
				{ 
					NewChunkPos = SingleChunkSpawnLocation;
				}

				float DistCamChunk = FGenericPlatformMath::Sqrt(FMath::Square(CameraChunkPosition.X - NewChunkPos.X) + FMath::Square(CameraChunkPosition.Y - NewChunkPos.Y));
				int32 Res = DistCamChunk > 5000 ? FMath::Floor(((0.0002*DistCamChunk)*MicroChunkResolution)/10.0)*10 : MicroChunkResolution;
				
				if(!OccupiedPositions.Contains(NewChunkPos))
				{
					if(!bCreateOnlyOne) { OccupiedPositions.Add(NewChunkPos); }
					AMCParent::SpawnMesh(NewChunkPos, Res);
					ChunkResolutions.Add(NewChunkPos, Res);
				}
				else if(ChunkResolutions.Contains(NewChunkPos))
				{
					if(ChunkResolutions[NewChunkPos] != Res)
					{
						ChunkResolutions.Remove(NewChunkPos);
						if(!bCreateOnlyOne) { OccupiedPositions.Add(NewChunkPos); }
						
						AMCubes* ChunkToUpdate = Cast<AMCubes>(Chunks[NewChunkPos]);
						ChunkToUpdate->MicroChunkResolution = Res;
						ChunkToUpdate->UpdateMesh();

						ChunkResolutions.Add(NewChunkPos, Res);
					}
				}

				// Since we visited it, add it to the list
				VisitedChunks.Add(ChunkCoord);
			}

			// Update Z value
			ZValue = ZValue == 0 ? -1 : (ZValue < 0 ? -ZValue : -ZValue - 1);

			// Have we visited all valid Z positions?
			if(-ZValue > RenderDistance || (bLimitRenderHeight && (ZValue < RenderHeightMin || ZValue > RenderHeightMax)))
			{
				// Yes! Reset Z

				ZValue = 0;

				// Check Loop State to see which variable must change

				if(XValue == MaxVal && LoopState == 0) { LoopState = 1; }
				else if(YValue == MaxVal && LoopState == 1) { LoopState = 2; }
				else if(XValue == -MaxVal && LoopState == 2) { LoopState = 3; }
				else if(YValue == -MaxVal && LoopState == 3) { LoopState = 0; MaxVal++; }

				// Deal with variable change

				switch(LoopState)
				{
					case 0:
						XValue++;
						break;
						
					case 1:
						YValue++;
						break;

					case 2:
						XValue--;
						break;

					case 3:
						YValue--;
						break;
				}
			}

			if(XValue > RenderDistance) { break; }

			if(bCreateOnlyOne) { break; }
		}

		CameraLastChunkPosition = CameraChunkPosition;
		LastFrequencies = Frequencies;
		LastAmplitudes = Amplitudes;
	}

	if(bFirstFrame) { bFirstFrame = false; }
	if(bCreateOnlyOnce) { AMCParent::SetActorTickEnabled(false); }
}

void AMCParent::SpawnMesh(const FVector& Location, const int32& MicroResolution)
{
	// UE_LOG(LogClass, Warning, TEXT("Spawning Mesh."));
	if(!bAlreadyCreated) { bAlreadyCreated = true; }
	
	{
		AMCubes* CubeMesh = AMCParent::GetWorld()->SpawnActorDeferred<AMCubes>(AMCubes::StaticClass(), FTransform(Location), this);
		CubeMesh->RootParent = this;
		CubeMesh->ChunkList = &Chunks;
		CubeMesh->TriTable = &TriTable;
		CubeMesh->EdgeTable = &EdgeTable;
		CubeMesh->BaseHeight = &BaseHeight;
		CubeMesh->WalkHeight = &WalkHeight;
		CubeMesh->CutoffPower = &CutoffPower;
		CubeMesh->MeshMaterial = MeshMaterial;
		CubeMesh->MeshXDimension = ChunkSize + 1;
		CubeMesh->MeshYDimension = ChunkSize + 1;
		CubeMesh->MeshZDimension = ChunkSize + 1;
		CubeMesh->MinimumCutoff = &MinimumCutoff;
		CubeMesh->MaximumCutoff = &MaximumCutoff;
		CubeMesh->ChunkOffset = &ChunkSpawnOffset;
		CubeMesh->RenderDistance = &RenderDistance;
		CubeMesh->SedimentWeight = &SedimentWeight;
		CubeMesh->IgnorePositions = &IgnorePositions;
		CubeMesh->OverhangPresence = &OverhangPresence;
		CubeMesh->SedimentFrequency = &SedimentFrequency;
		CubeMesh->MicroChunkResolution = MicroResolution;
		CubeMesh->FinishSpawning(FTransform(Location));

		Chunks.Add(Location, Cast<AActor>(CubeMesh));
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