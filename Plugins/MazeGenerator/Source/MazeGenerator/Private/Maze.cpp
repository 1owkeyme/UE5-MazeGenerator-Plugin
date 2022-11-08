// Copyright LowkeyMe. All Rights Reserved.


#include "Maze.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Algorithms/Algorithm.h"
#include "Algorithms/Kruskal.h"
#include "Algorithms/Prim.h"

AMaze::AMaze()
{
	PrimaryActorTick.bCanEverTick = false;

	GenerationAlgorithms.Add(EGenerationAlgorithm::Kruskal, TSharedPtr<Algorithm>(new Kruskal));
	GenerationAlgorithms.Add(EGenerationAlgorithm::Prim, TSharedPtr<Algorithm>(new Prim));

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
}

void AMaze::GenerateMaze()
{
	ClearMaze();

	FloorCell.CalculateSize();
	WallCell.CalculateSize();

	if (!(FloorCell && WallCell))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not all cells for maze have been specified."));
		return;
	}

	FloorCells->SetStaticMesh(FloorCell.StaticMesh);
	WallCells->SetStaticMesh(WallCell.StaticMesh);


	const TSharedPtr<Algorithm> ChosenAlgorithm = GenerationAlgorithms[GenerationAlgorithm];

	const TArray<TArray<uint8>> Grid = ChosenAlgorithm->GetGrid(FIntVector2(MazeSize), Seed);

	const FVector2D FloorSize = FloorCell.GetSize();
	const FVector2D WallSize = WallCell.GetSize();
	for (int X = 0; X < MazeSize.X; ++X)
	{
		for (int Y = 0; Y < MazeSize.Y; ++Y)
		{
			if (Grid[X][Y])
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

void AMaze::ClearMaze() const
{
	FloorCells->ClearInstances();
	WallCells->ClearInstances();
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
