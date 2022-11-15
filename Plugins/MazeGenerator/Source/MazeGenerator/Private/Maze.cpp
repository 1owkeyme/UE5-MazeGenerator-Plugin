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

void AMaze::UpdateMaze()
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
		CreateMazeOutline();
	}
	if (PathFloorCell)
	{
		PathFloorCells->SetStaticMesh(PathFloorCell.StaticMesh);
	}

	MazeGrid = GenerationAlgorithms[GenerationAlgorithm]->GetGrid(MazeSize, Seed);

	PathStart.ClampByMazeSize(MazeSize);
	PathEnd.ClampByMazeSize(MazeSize);
	MazePath = GetMazePath(PathStart, PathEnd);


	for (int32 Y = 0; Y < MazeSize.Y; ++Y)
	{
		for (int32 X = 0; X < MazeSize.X; ++X)
		{
			if (bShowPath && MazePath.Grid.Num() > 0 && MazePath.Grid[Y][X])
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

void AMaze::CreateMazeOutline() const
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

FMazePath AMaze::GetMazePath(const FMazeCoordinates& Start, const FMazeCoordinates& End)
{
	TArray<TArray<int32>> Graph;
	Graph.Reserve(MazeGrid.Num() * MazeGrid[0].Num());

	// Graph creation.
	for (int32 GraphVertex,
	           Y = 0; Y < MazeGrid.Num(); ++Y)
	{
		for (int32 X = 0; X < MazeGrid[Y].Num(); ++X)
		{
			GraphVertex = Y * MazeGrid[Y].Num() + X;

			Graph.Emplace(TArray<int32>());
			if (!MazeGrid[Y][X])
			{
				continue;
			}

			Graph[GraphVertex].Reserve(4); // There are only 4 directions possible.

			if (X > 0 && MazeGrid[Y][X - 1]) // West direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex - 1);
			}
			if (X + 1 < MazeGrid[Y].Num() && MazeGrid[Y][X + 1]) // East direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex + 1);
			}
			if (Y > 0 && MazeGrid[Y - 1][X]) // North direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex - MazeGrid[Y].Num());
			}
			if (Y + 1 < MazeGrid.Num() && MazeGrid[Y + 1][X]) // South direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex + MazeGrid[Y].Num());
			}

			Graph[GraphVertex].Shrink();
		}
	}

	const int32 StartVertex = Start.Y * MazeGrid[0].Num() + Start.X;
	const int32 EndVertex = End.Y * MazeGrid[0].Num() + End.X;


	TQueue<int32> Vertices;

	const int32 VerticesAmount = MazeGrid.Num() * MazeGrid[0].Num();

	TArray<bool> Visited;
	Visited.Init(false, VerticesAmount);

	TArray<int32> Parents;
	Parents.Init(-1, VerticesAmount);

	TArray<int32> Distances;
	Distances.Init(0, VerticesAmount);

	int32 Vertex;
	Vertices.Enqueue(StartVertex);
	Visited[StartVertex] = true;
	while (Vertices.Dequeue(Vertex))
	{
		for (int32 i = 0; i < Graph[Vertex].Num(); ++i)
		{
			if (const int32 Adjacent = Graph[Vertex][i]; !Visited[Adjacent])
			{
				Visited[Adjacent] = true;
				Vertices.Enqueue(Adjacent);
				Distances[Adjacent] = Distances[Vertex] + 1;
				Parents[Adjacent] = Vertex;
			}
		}
	}

	TArray<int32> GraphPath;
	if (!Visited[EndVertex])
	{
		UE_LOG(LogTemp, Warning, TEXT("Path is not reachable."));
		return FMazePath();
	}

	for (int VertexNumber = EndVertex; VertexNumber != -1; VertexNumber = Parents[VertexNumber])
	{
		GraphPath.Emplace(VertexNumber);
	}

	Algo::Reverse(GraphPath);

	FMazePath Path;
	Path.Grid.Init(TArray<uint8>(), MazeGrid.Num());
	for (int Y = 0; Y < MazeGrid.Num(); ++Y)
	{
		Path.Grid[Y].SetNumZeroed(MazeGrid[Y].Num());
	}

	for (int32 VertexNumber, i = 0; i < GraphPath.Num(); ++i)
	{
		VertexNumber = GraphPath[i];

		Path.Grid[VertexNumber / MazeGrid[0].Num()][VertexNumber % MazeGrid[0].Num()] = 1;
	}

	Path.Length = Distances[EndVertex] + 1;
	return Path;
}

void AMaze::ClearMaze() const
{
	FloorCells->ClearInstances();
	WallCells->ClearInstances();
	OutlineWallCells->ClearInstances();
	PathFloorCells->ClearInstances();
}

void AMaze::Randomize()
{
	MazeSize.X = FMath::RandRange(3, 101) | 1; // | 1 to make odd.
	MazeSize.Y = FMath::RandRange(3, 101) | 1;

	TArray<EGenerationAlgorithm> Algorithms;
	const int32 Num = GenerationAlgorithms.GetKeys(Algorithms);
	GenerationAlgorithm = Algorithms[FMath::RandRange(0, Num - 1)];

	Seed = FMath::RandRange(0, MAX_int32 - 1);

	PathStart.X = 0;
	PathStart.X = 0;
	PathEnd.X = MazeSize.X - 1;
	PathEnd.Y = MazeSize.Y - 1;

	UpdateMaze();
}

void AMaze::BeginPlay()
{
	Super::BeginPlay();
}

void AMaze::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (Transform.Equals(LastMazeTransform))
	{
#endif
		UpdateMaze();
#if WITH_EDITOR
	}
	LastMazeTransform = Transform;
#endif
}
