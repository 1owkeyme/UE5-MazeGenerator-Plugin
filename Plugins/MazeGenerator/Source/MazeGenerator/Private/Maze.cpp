// Copyright LowkeyMe. All Rights Reserved.


#include "Maze.h"


#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Algorithms/Algorithm.h"
#include "Algorithms/Backtracker.h"
#include "Algorithms/Division.h"
#include "Algorithms/Eller.h"
#include "Algorithms/HaK.h"
#include "Algorithms/Kruskal.h"
#include "Algorithms/Prim.h"
#include "Algorithms/Sidewinder.h"
#include "Pathfinder.h"

void FMazeCoordinates::ClampByMazeSize(const FMazeSize& MazeSize)
{
	if (X >= MazeSize.X)
	{
		X = MazeSize.X - 1;
	}
	if (Y >= MazeSize.Y)
	{
		Y = MazeSize.Y - 1;
	}
}

AMaze::AMaze()
{
	PrimaryActorTick.bCanEverTick = false;

	GenerationAlgorithms.Add(EGenerationAlgorithm::Backtracker, TSharedPtr<Algorithm>(new Backtracker));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Division, TSharedPtr<Algorithm>(new Division));
	GenerationAlgorithms.Add(EGenerationAlgorithm::HaK, TSharedPtr<Algorithm>(new HaK));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Sidewinder, TSharedPtr<Algorithm>(new Sidewinder));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Kruskal, TSharedPtr<Algorithm>(new Kruskal));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Eller, TSharedPtr<Algorithm>(new Eller));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Prim, TSharedPtr<Algorithm>(new Prim));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	FloorCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloorCells"));
	if (FloorCells)
	{
		FloorCells->SetupAttachment(GetRootComponent());
	}

	PathFloorCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("PathFloorCells"));
	if (PathFloorCells)
	{
		PathFloorCells->SetupAttachment(GetRootComponent());
	}

	WallCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallCells"));
	if (WallCells)
	{
		WallCells->SetupAttachment(GetRootComponent());
	}

	OutlineWallCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("OutlineWallCells"));
	if (OutlineWallCells)
	{
		OutlineWallCells->SetupAttachment(GetRootComponent());
	}
}

void AMaze::GenerateMaze(const bool bShouldUpdateMaze)
{
	ClearMaze();

	if (!(FloorCell && WallCell))
	{
		UE_LOG(LogTemp, Warning, TEXT("To create maze specify FloorCell and WallCell."));
		return;
	}

	FloorCells->SetStaticMesh(FloorCell.StaticMesh);
	WallCells->SetStaticMesh(WallCell.StaticMesh);

	MazeCellSize = FloorCell.GetSize() > WallCell.GetSize() ? FloorCell.GetSize() : WallCell.GetSize();

	if (OutlineWallCell)
	{
		OutlineWallCells->SetStaticMesh(OutlineWallCell.StaticMesh);
		if (const FVector2D CellSize = OutlineWallCell.GetSize(); CellSize > MazeCellSize)
		{
			MazeCellSize = CellSize;
		}
		
		GenerateMazeOutline();
	}

	if (PathFloorCell)
	{
		PathFloorCells->SetStaticMesh(PathFloorCell.StaticMesh);
	}

	if (bShouldUpdateMaze)
	{
		MazeGrid = GenerationAlgorithms[GenerationAlgorithm]->GetGrid(FIntVector2(MazeSize), Seed);
		if (bEnablePathfinder)
		{
			PathStart.ClampByMazeSize(MazeSize);
			PathEnd.ClampByMazeSize(MazeSize);
			MazePathGrid = Pathfinder::FindPath(MazeGrid,
			                                    TPair<int32, int32>(PathStart),
			                                    TPair<int32, int32>(PathEnd));
		}
	}

	for (int32 Y = 0; Y < MazeSize.Y; ++Y)
	{
		for (int32 X = 0; X < MazeSize.X; ++X)
		{
			if (bEnablePathfinder && MazePathGrid.Num() > 0 && MazePathGrid[Y][X])
			{
				const FVector Location(MazeCellSize.X * X, MazeCellSize.Y * Y, 0.f);
				PathFloorCells->AddInstance(FTransform(Location));
			}
			else if (MazeGrid[Y][X])
			{
				const FVector Location(MazeCellSize.X * X, MazeCellSize.Y * Y, 0.f);
				FloorCells->AddInstance(FTransform(Location));
			}
			else
			{
				const FVector Location(MazeCellSize.X * X, MazeCellSize.Y * Y, 0.f);
				WallCells->AddInstance(FTransform(Location));
			}
		}
	}
}

void AMaze::GenerateMazeOutline() const
{
	FVector Location1;
	FVector Location2;

	Location1.Y = -MazeCellSize.Y;
	Location2.Y = MazeCellSize.Y * MazeSize.Y;
	for (int32 X = -1; X < MazeSize.X + 1; ++X)
	{
		Location1.X = Location2.X = X * MazeCellSize.X;
		OutlineWallCells->AddInstance(FTransform(Location1));
		OutlineWallCells->AddInstance(FTransform(Location2));
	}

	Location1.X = -MazeCellSize.X;
	Location2.X = MazeCellSize.X * MazeSize.X;
	for (int32 Y = 0; Y < MazeSize.Y; ++Y)
	{
		Location1.Y = Location2.Y = Y * MazeCellSize.Y;
		OutlineWallCells->AddInstance(FTransform(Location1));
		OutlineWallCells->AddInstance(FTransform(Location2));
	}
}

void AMaze::ClearMaze() const
{
	FloorCells->ClearInstances();
	WallCells->ClearInstances();
	OutlineWallCells->ClearInstances();
	PathFloorCells->ClearInstances();
}

void AMaze::BeginPlay()
{
	Super::BeginPlay();

	if (!bGenerateInEditor)
	{
		GenerateMaze();
	}
}

void AMaze::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);


	if (bGenerateInEditor)
	{
		const bool bShouldUpdate = Transform.Equals(LastMazeTransform) || !MazeGrid.Num();
		GenerateMaze(bShouldUpdate);
	}
	else
	{
		ClearMaze();
	}

	LastMazeTransform = Transform;
}
