#include "Pathfinder.h"


TArray<TArray<uint8>> Pathfinder::FindPath(const TArray<TArray<uint8>>& Maze,
                                           const TPair<int32, int32>& Start, const TPair<int32, int32>& End)
{
	TArray<TArray<int32>> Graph;
	Graph.Reserve(Maze.Num() * Maze[0].Num());

	// Graph creation.
	for (int32 GraphVertex,
	           Y = 0; Y < Maze.Num(); ++Y)
	{
		for (int32 X = 0; X < Maze[Y].Num(); ++X)
		{
			GraphVertex = Y * Maze[Y].Num() + X;

			Graph.Emplace(TArray<int32>());
			if (!Maze[Y][X])
			{
				continue;
			}

			Graph[GraphVertex].Reserve(4); // There are only 4 directions possible.

			if (X > 0 && Maze[Y][X - 1]) // West direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex - 1);
			}
			if (X + 1 < Maze[Y].Num() && Maze[Y][X + 1]) // East direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex + 1);
			}
			if (Y > 0 && Maze[Y - 1][X]) // North direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex - Maze[Y].Num());
			}
			if (Y + 1 < Maze.Num() && Maze[Y + 1][X]) // South direction.
			{
				Graph[GraphVertex].Emplace(GraphVertex + Maze[Y].Num());
			}

			Graph[GraphVertex].Shrink();
		}
	}

	const int32 StartVertex = Start.Value * Maze[0].Num() + Start.Key;
	const int32 EndVertex = End.Value * Maze[0].Num() + End.Key;


	TQueue<int32> Vertices;

	const int32 VerticesAmount = Maze.Num() * Maze[0].Num();

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
		return TArray<TArray<uint8>>();
	}

	for (int VertexNumber = EndVertex; VertexNumber != -1; VertexNumber = Parents[VertexNumber])
	{
		GraphPath.Emplace(VertexNumber);
	}

	Algo::Reverse(GraphPath);

	TArray<TArray<uint8>> MazePath;
	MazePath.Init(TArray<uint8>(), Maze.Num());
	for (int Y = 0; Y < Maze.Num(); ++Y)
	{
		MazePath[Y].SetNumZeroed(Maze[Y].Num());
	}

	for (int32 VertexNumber, i = 0; i < GraphPath.Num(); ++i)
	{
		VertexNumber = GraphPath[i];

		MazePath[VertexNumber / Maze[0].Num()][VertexNumber % Maze[0].Num()] = 1;
	}

	return MazePath;
}
