// Copyright LowkeyMe. All Rights Reserved.


#include "Maze.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Algorithms/Algorithm.h"
#include "Algorithms/Backtracker.h"
#include "Algorithms/Division.h"
#include "Algorithms/Eller.h"
#include "Algorithms/Kruskal.h"
#include "Algorithms/Prim.h"

AMaze::AMaze()
{
	PrimaryActorTick.bCanEverTick = false;

	GenerationAlgorithms.Add(EGenerationAlgorithm::Kruskal, TSharedPtr<Algorithm>(new Kruskal));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Prim, TSharedPtr<Algorithm>(new Prim));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Backtracker, TSharedPtr<Algorithm>(new Backtracker));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Eller, TSharedPtr<Algorithm>(new Eller));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Division, TSharedPtr<Algorithm>(new Division));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	FloorCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloorCells"));
	if (FloorCells)
	{
		FloorCells->SetupAttachment(GetRootComponent());
		FloorCells->SetStaticMesh(FloorCell.StaticMesh);
	}

	WallCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallCells"));
	if (WallCells)
	{
		WallCells->SetupAttachment(GetRootComponent());
		WallCells->SetStaticMesh(WallCell.StaticMesh);
	}

	OutlineWallCells = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("OutlineWallCells"));
	if (OutlineWallCells)
	{
		OutlineWallCells->SetupAttachment(GetRootComponent());
		OutlineWallCells->SetStaticMesh(OutlineWallCell.StaticMesh);
	}
}

void AMaze::GenerateMaze()
{
	ClearMaze();

	FloorCell.CalculateSize();
	WallCell.CalculateSize();

	if (!(FloorCell && WallCell))
	{
		UE_LOG(LogTemp, Warning, TEXT("To create maze specify FloorCell and WallCell."));
		return;
	}

	FloorCells->SetStaticMesh(FloorCell.StaticMesh);
	WallCells->SetStaticMesh(WallCell.StaticMesh);

	if (bGenerateOutline)
	{
		if (OutlineWallCell)
		{
			OutlineWallCells->SetStaticMesh(OutlineWallCell.StaticMesh);
			GenerateMazeOutline();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("To create outline for maze specify OutlineWallCell."));
		}
	}

	const TSharedPtr<Algorithm> ChosenAlgorithm = GenerationAlgorithms[GenerationAlgorithm];

	const TArray<TArray<uint8>> Grid = ChosenAlgorithm->GetGrid(FIntVector2(MazeSize), Seed);

	const FVector2D FloorSize = FloorCell.GetSize();
	const FVector2D WallSize = WallCell.GetSize();
	for (int32 Y = 0; Y < MazeSize.Y; ++Y)
	{
		for (int32 X = 0; X < MazeSize.X; ++X)
		{
			if (Grid[Y][X])
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
}

void AMaze::BeginPlay()
{
	Super::BeginPlay();

	if (!bGenerateInEditor)
	{
		GenerateMaze();
	}
}

void AMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMaze::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bGenerateInEditor)
	{
		GenerateMaze();
	}
	else
	{
		ClearMaze();
	}
}
