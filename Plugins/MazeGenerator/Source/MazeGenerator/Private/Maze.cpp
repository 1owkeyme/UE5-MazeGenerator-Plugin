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

void AMaze::GenerateMaze(const bool bShouldUpdate)
{
	ClearMaze();

	if (!(FloorCell && WallCell))
	{
		UE_LOG(LogTemp, Warning, TEXT("To create maze specify FloorCell and WallCell."));
		return;
	}

	FloorCells->SetStaticMesh(FloorCell.StaticMesh);
	FloorCell.CalculateSize();

	WallCells->SetStaticMesh(WallCell.StaticMesh);
	WallCell.CalculateSize();

	if (OutlineWallCell && bCreateOutline)
	{
		OutlineWallCells->SetStaticMesh(OutlineWallCell.StaticMesh);
		OutlineWallCell.CalculateSize();

		GenerateMazeOutline();
	}

	if (PathFloorCell)
	{
		PathFloorCells->SetStaticMesh(PathFloorCell.StaticMesh);
		PathFloorCell.CalculateSize();
	}

	if (bShouldUpdate)
	{
		MazeGrid = GenerationAlgorithms[GenerationAlgorithm]->GetGrid(FIntVector2(MazeSize), Seed);
		if (bEnablePathfinder)
		{
			PathStart.ClampByMazeSize(MazeSize);
			PathEnd.ClampByMazeSize(MazeSize);
			MazePathGrid = Pathfinder::FindPath(MazeGrid,
			                                    TPair<int32, int32>(PathStart),
			                                    TPair<int32, int32>(PathEnd));
			if (!MazePathGrid.Num())
			{
				UE_LOG(LogTemp, Warning,
				       TEXT("To create path make sure Start and End are not walls and in bounds of Maze Size."));
			}
		}
	}


	const FVector2D FloorSize = FloorCell.GetSize();
	const FVector2D PathFloorSize = PathFloorCell.GetSize();
	const FVector2D WallSize = WallCell.GetSize();
	for (int32 Y = 0; Y < MazeSize.Y; ++Y)
	{
		for (int32 X = 0; X < MazeSize.X; ++X)
		{
			if (bEnablePathfinder && MazePathGrid.Num() > 0 && MazePathGrid[Y][X])
			{
				const FVector Location(PathFloorSize.X * X, PathFloorSize.Y * Y, 0.f);
				PathFloorCells->AddInstance(FTransform(Location));
			}
			else if (MazeGrid[Y][X])
			{
				const FVector Location(FloorSize.X * X, FloorSize.Y * Y, 0.f);
				FloorCells->AddInstance(FTransform(Location));
			}
			else
			{
				const FVector Location(WallSize.X * X, WallSize.Y * Y, 0.f);
				WallCells->AddInstance(FTransform(Location));
			}
		}
	}
}

void AMaze::GenerateMazeOutline()
{
	FVector Location1;
	FVector Location2;

	Location1.Y = -WallCell.GetSize().Y;
	Location2.Y = WallCell.GetSize().Y * MazeSize.Y;
	for (int32 X = -1; X < MazeSize.X + 1; ++X)
	{
		Location1.X = Location2.X = X * WallCell.GetSize().X;
		OutlineWallCells->AddInstance(FTransform(Location1));
		OutlineWallCells->AddInstance(FTransform(Location2));
	}

	Location1.X = -WallCell.GetSize().X;
	Location2.X = WallCell.GetSize().X * MazeSize.X;
	for (int32 Y = 0; Y < MazeSize.Y; ++Y)
	{
		Location1.Y = Location2.Y = Y * WallCell.GetSize().Y;
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

	const bool bShouldUpdate = Transform.Equals(LastMazeTransform) || !MazeGrid.Num();
	if (bGenerateInEditor)
	{
		GenerateMaze(bShouldUpdate);
	}
	else
	{
		ClearMaze();
	}
	LastMazeTransform = Transform;
}
