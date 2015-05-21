

#include "Scroller2D.h"
#include "Tileset2D.h"
#include "Tile2D.h"
#include "CoreMisc.h"



ATileset2D::ATileset2D(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

	UE_LOG(MyLog, Warning, TEXT("Tileset2D is loading"))
	//Get the blueprints to spawn in the world
	static ConstructorHelpers::FObjectFinder<UBlueprint> TopLeftBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/TLeftBoundary.TLeftBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> TopRightBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/TRightBoundary.TRightBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> BottomLeftBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/BLeftBoundary.BLeftBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> BottomRightBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/BRightBoundary.BRightBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> LeftBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/LeftBoundary.LeftBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> RightBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/RightBoundary.RightBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> TopBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/TBoundary.TBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> BottomBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/BBoundary.BBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> MazeBoundaryAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/MiddleBoundary.MiddleBoundary'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> MazeFloorAsset(TEXT("Blueprint'/Game/Tiles/TileBlueprints/ForestFloor.ForestFloor'"));


	if (TopLeftBoundaryAsset.Object != NULL)
	{
		TopLeftBoundary = (UClass*)TopLeftBoundaryAsset.Object->GeneratedClass;
	}

	if (TopRightBoundaryAsset.Object != NULL)
	{
		TopRightBoundary = (UClass*)TopRightBoundaryAsset.Object->GeneratedClass;
	}

	if (BottomLeftBoundaryAsset.Object != NULL)
	{
		BottomLeftBoundary = (UClass*)BottomLeftBoundaryAsset.Object->GeneratedClass;
	}
	
	if (BottomRightBoundaryAsset.Object != NULL)
	{
		BottomRightBoundary = (UClass*)BottomRightBoundaryAsset.Object->GeneratedClass;
	}

	if (LeftBoundaryAsset.Object != NULL)
	{
		LeftBoundary = (UClass*)LeftBoundaryAsset.Object->GeneratedClass;
	}

	if (RightBoundaryAsset.Object != NULL)
	{
		RightBoundary = (UClass*)RightBoundaryAsset.Object->GeneratedClass;
	}

	if (TopBoundaryAsset.Object != NULL)
	{
		TopBoundary = (UClass*)TopBoundaryAsset.Object->GeneratedClass;
	}

	if (BottomBoundaryAsset.Object != NULL)
	{
		BottomBoundary = (UClass*)BottomBoundaryAsset.Object->GeneratedClass;
	}

	if (MazeBoundaryAsset.Object != NULL)
	{
		MazeBoundary = (UClass*)MazeBoundaryAsset.Object->GeneratedClass;
	}

	if (MazeFloorAsset.Object != NULL)
	{
		MazeFloor = (UClass*)MazeFloorAsset.Object->GeneratedClass;
	}

	Offset = 32.0f;

	IsGenerated = false;

	PrimaryActorTick.bCanEverTick = true;

	GridColor = FColor::Red;

	MapFileName = "map1";

	bPathfindingInUse = false;

	bTrimPath = false;

	bShowGrid = false;
	
}

void ATileset2D::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShowGrid)
	{
		Grid.DrawEdges(GetWorld(), GridColor);
	}
}

//Function to load the map to a TArray of FStrings
void ATileset2D::LoadMapFile()
{
	
	FString FilePath = FPaths::GameDir() + "Content/MapFiles/" + MapFileName + ".txt";

	if (!FFileHelper::LoadANSITextFileToStrings(*FilePath, NULL, MapStrings))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Could not find file "));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FilePath);

	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("File loaded!!!"));
		
		CreateMap();
	}
}


void ATileset2D::AddEdges()
{
	//ATile2D * node = NULL;
	//UE_LOG(MyLog, Log, TEXT("***************ADD EDGES START***************"));
	FString moveType = "";
	bool isEdgePossible = true;
	int32 numEdges = 0;

	for (int32 x = 0; x < rows; x++)
	{
		for (int32 y = 0; y < columns; y++)
		{
			FVector start(x * Offset, 0, -y * Offset);
			 
			if (Grid.findVertex(start))
			{
				numEdges = 0;
				//If the tile at position x,y is walkable
				//then add the edges for that tile
				for (int32 u = 0; u < 3; u++)
				{
					for (int32 v = 0; v < 3; v++)
					{
						FVector end(start.X + (-Offset + (u * Offset)), 0, start.Z + (Offset - (v * Offset)));
						if ((u == 0 && v == 0) || (u == 2 && v == 0) || (u == 0 && v == 2) || (u == 2 && v == 2))
						{
							moveType = "diagonal";
							if (u == 0 && v == 0)
							{
								if (!Grid.findVertex(FVector(end.X, 0, end.Z - Offset)) || !Grid.findVertex(FVector(end.X + Offset, 0, end.Z)))
								{
									isEdgePossible = false;
								}
							}
							else if (u == 2 && v == 0)
							{
								if (!Grid.findVertex(FVector(end.X - Offset, 0, end.Z)) || !Grid.findVertex(FVector(end.X, 0, end.Z - Offset)))
								{
									isEdgePossible = false;
								}
							}
							else if (u == 0 && v == 2)
							{
								if (!Grid.findVertex(FVector(end.X, 0, end.Z + Offset)) || !Grid.findVertex(FVector(end.X + Offset, 0, end.Z)))
								{
									isEdgePossible = false;
								}
							}
							else if (u == 2 && v == 2)
							{
								if (!Grid.findVertex(FVector(end.X, 0, end.Z + Offset)) || !Grid.findVertex(FVector(end.X - Offset, 0, end.Z)))
								{
									isEdgePossible = false;
								}
							}
							
						}
						else
						{
							moveType = "vertical";
							isEdgePossible = true;
						}

						if (isEdgePossible)
						{
							Grid.addEdge(start, end, moveType, Offset);
							numEdges++;
						}
						//UE_LOG(MyLog, Log, TEXT("***************ADDING EDGE***************"));
					}
				}

				if (numEdges < 8)
				{
					Grid.findVertex(start)->bIsValidPathPoint = true;
				}
			}
		}
	}
	//UE_LOG(MyLog, Log, TEXT("***************ADD EDGES END***************"));

}

void ATileset2D::AddVertex(FVector Loc)
{
	Grid.addVertex(Loc);
}



void ATileset2D::CreateMap()
{
	if (!IsGenerated)
	{
		//UE_LOG(YourLog, Log, TEXT("**********BUILD MAP START***************"));
		FString line = "";

		FActorSpawnParameters SpawnInfo;

		SpawnInfo.bNoCollisionFail = true;
		SpawnInfo.Owner = this;
		SpawnInfo.bDeferConstruction = false;



		//Get number of rows and columns
		columns = MapStrings.Num();
		rows = MapStrings[0].Len();

		line.Empty(0);

		if (GetWorld())
		{
			ATile2D* NewTile = NULL;

			//Iterate through the rest of the file and build the map
			for (int32 i = 0; i < MapStrings.Num(); i++)
			{
				line = MapStrings[i];
				for (int32 x = 0; x < line.Len(); x++)
				{
					FVector SpawnLoc(x * Offset, 0.0f, -i * Offset);
					FRotator SpawnRot(0.0f, 0.0f, 0.0f);

					

					if (line[x] == 'T' && x == 0)
					{
						//spawn top left border
						NewTile = GetWorld()->SpawnActor<ATile2D>(TopLeftBoundary, SpawnLoc, SpawnRot, SpawnInfo);
					}
					else if (line[x] == 'T' && x == line.Len() - 1)
					{
						//spawn top right border
						NewTile = GetWorld()->SpawnActor<ATile2D>(TopRightBoundary, SpawnLoc, SpawnRot, SpawnInfo);
					}
					else if (line[x] == 'B' && x == 0)
					{
						//spawn bottom left border
						NewTile = GetWorld()->SpawnActor<ATile2D>(BottomLeftBoundary, SpawnLoc, SpawnRot, SpawnInfo);

					}
					else if (line[x] == 'B' && x == line.Len() - 1)
					{
						//spawn bottom right border
						NewTile = GetWorld()->SpawnActor<ATile2D>(BottomRightBoundary, SpawnLoc, SpawnRot, SpawnInfo);
					}
					else if (line[x] == 't')
					{
						//spawn top border
						NewTile = GetWorld()->SpawnActor<ATile2D>(TopBoundary, SpawnLoc, SpawnRot, SpawnInfo);
					}
					else if (line[x] == 'l')
					{
						//spawn left border
						NewTile = GetWorld()->SpawnActor<ATile2D>(LeftBoundary, SpawnLoc, SpawnRot, SpawnInfo);
					}
					else if (line[x] == 'r')
					{
						NewTile = GetWorld()->SpawnActor<ATile2D>(RightBoundary, SpawnLoc, SpawnRot, SpawnInfo);
						//spawn right border
					}
					else if (line[x] == 'b')
					{
						//spawn bottom border
						NewTile = GetWorld()->SpawnActor<ATile2D>(BottomBoundary, SpawnLoc, SpawnRot, SpawnInfo);
					}
					else if (line[x] == 'M')
					{
						//spawn maze boundary
						NewTile = GetWorld()->SpawnActor<ATile2D>(MazeBoundary, SpawnLoc, SpawnRot, SpawnInfo);

					}
					else
					{
						//spawn floor tile
						NewTile = GetWorld()->SpawnActor<ATile2D>(MazeFloor, SpawnLoc, SpawnRot, SpawnInfo);
					}

					//
					if (NewTile)
					{
						if (NewTile->IsWalkable)
						{
							AddVertex(SpawnLoc);
						}
						else
						{
							//NewTile->Sphere->SetSphereRadius(FMath::CeilToFloat(FMath::Sqrt(FMath::Pow(Offset / 2, 2) * 2)));
						}
					}
				}
				line = "";

			}

			IsGenerated = true;

			AddEdges();

			/*****This section is for testing purposes only*****/
			
			//Grid.DrawNodes(GetWorld(), FColor::Red);
			//Grid.DrawEdges(GetWorld(), FColor::Red);
			//UE_LOG(YourLog, Log, TEXT("*****************BUILD MAP END*******************"));
			/*******End testing section**********************/
		}
		
		
	}
}

void ATileset2D::ShowPath(FVector start, FVector end)
{
	Grid.ShowPath(GetWorld(), end);

}


void ATileset2D::FindPath(FVector start, FVector end, TArray<FVector> & path)
{
	//UE_LOG(MyLog, Log, TEXT("NAVIGATION FIND PATH: START!!!"));

	bPathfindingInUse = true;
	Grid.FindPath(start, end, FMath::CeilToFloat(FMath::Sqrt(FMath::Pow(Offset / 2, 2) * 2)));

	Grid.GetPath(end, path, bTrimPath);

	bPathfindingInUse = false;

	//UE_LOG(MyLog, Log, TEXT("NAVIGATION FIND PATH: END!!!"));

}


void ATileset2D::CreateBorders()
{
	
	for (int32 x = 0; x < rows; x++)
	{
		for (int32 y = 0; y < columns; y++)
		{
			FVector pos(x * Offset, 0, -y * Offset);
			if (Grid.findVertex(pos))
			{
				//UE_LOG(MyLog, Log, TEXT("Vertex: %s"), *pos.ToString());

				//row
				for (int32 u = 0; u < 3; u++)
				{
					//column
					for (int32 v = 0; v < 3; v++)
					{
						FVector end(pos.X + (-Offset + (u * Offset)), 0, pos.Z + (Offset - (v * Offset)));
						if (!Grid.findVertex(end))
						{
							FVector newPos(pos.X - Offset / 2, 0, pos.Z + Offset / 2);

							if (u == 1 && v == 0)
							{
								//add vertices at top corners
								
								AddVertex(newPos);
								newPos.X += Offset;

								AddVertex(newPos);
							}
							else if (u == 2 && v == 1)
							{
								//add vertices at right side corners
								newPos.X += Offset;
								AddVertex(newPos);

								newPos.Z -= Offset;
								AddVertex(newPos);

							}
							else if (u == 0 && v == 1)
							{
								//add vertices at left side corners
								AddVertex(newPos);

								newPos.Z -= Offset;
								
								AddVertex(newPos);
							}
							else if (u == 1 && v == 2)
							{
								//add vertices to bottom
								newPos.Z -= Offset;
								AddVertex(newPos);

								newPos.X += Offset;
								AddVertex(newPos);
							}
							else
							{
								//don't do anything
							}
						}
					}
				}
			}
		}
	}
}

void ATileset2D::TrimPath(TArray<FVector> & path)
{
	if (path.Num() > 3)
	{
		for (int32 i = 1; i < path.Num() - 1; i++)
		{
			FVector node = path[i];
			if (Grid.findVertex(node + FVector(0, -1, 0)))
			{				
				if (!Grid.findVertex(node + FVector(0, -1, 0))->bIsValidPathPoint)
				{
					path.Remove(node);
				}
			}
			else
			{
				UE_LOG(PathFollowing, Warning, TEXT("node not found!!!"));
			}
		}
		UE_LOG(PathFollowing, Log, TEXT("To %d"), path.Num());

	}
}