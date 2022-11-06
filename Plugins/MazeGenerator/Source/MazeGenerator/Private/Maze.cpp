// Copyright LowkeyMe. All Rights Reserved.


#include "Maze.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

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


	const TSharedPtr<Algorithm> Algorithm = GenerationAlgorithms[GenerationAlgorithm];

	FGrid Grid = Algorithm->GetGrid(FVector2D(MazeSize));
	
	for (int X = 0; X < MazeSize.X; ++X)
	{
		for (int Y = 0; Y < MazeSize.Y; ++Y)
		{
			FTransform Transform(FVector(FloorCell.GetSize().X * X, FloorCell.GetSize().Y * Y, 0.f));

			FloorCells->AddInstance(Transform);
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
