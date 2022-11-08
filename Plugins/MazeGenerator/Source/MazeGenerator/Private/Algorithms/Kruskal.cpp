// Copyright LowkeyMe. All Rights Reserved.


#include "Kruskal.h"

#include "Utils.h"

Tree* Tree::GetRoot()
{
	if (Parent)
	{
		return Parent->GetRoot();
	}
	return this;
}

bool Tree::IsConnected(Tree* const AnotherTree)
{
	if (GetRoot() == AnotherTree->GetRoot())
	{
		return true;
	}
	return false;
}

void Tree::Connect(Tree* const TreeToConnect)
{
	TreeToConnect->GetRoot()->Parent = this;
}

TArray<TArray<uint8>> Kruskal::GetGrid(const FIntVector2& Size, const int32 Seed)
{
	// There is for each 2 not connected floors 1 wall between.
	const FIntVector2 DirectionsGridSize((Size.X + 1) / 2, (Size.Y + 1) / 2);

	TArray<TArray<uint8>> DirectionsGrid = GetDirectionsGrid(DirectionsGridSize, Seed);

	TArray<TArray<uint8>> Grid;
	Grid.Init(TArray<uint8>(), Size.X);
	for (int32 X = 0; X < Size.X; ++X)
	{
		Grid[X].SetNumZeroed(Size.Y);
	}

	// Remap directions grid onto regular grid.
	for (int32 X = 0; X < DirectionsGridSize.X; ++X)
	{
		for (int32 Y = 0; Y < DirectionsGridSize.Y; ++Y)
		{
			Grid[X * 2][Y * 2] = 1;

			//It only makes sense to check the western and northern directions,
			//because the remaining ones would simply overlap.

			if (DirectionsGrid[X][Y] & static_cast<uint8>(EDirection::West))
			{
				Grid[X * 2 - 1][Y * 2] = 1;
			}
			if (DirectionsGrid[X][Y] & static_cast<uint8>(EDirection::North))
			{
				Grid[X * 2][Y * 2 - 1] = 1;
			}
		}
	}

	return Grid;
}

TArray<TArray<uint8>> Kruskal::GetDirectionsGrid(const FIntVector2& Size, const int32 Seed) const
{
	TArray<TArray<uint8>> Grid;
	Grid.Init(TArray<uint8>(), Size.X);
	for (int32 X = 0; X < Size.X; ++X)
	{
		Grid[X].SetNumZeroed(Size.Y);
	}

	TArray<TArray<TSharedPtr<Tree>>> Trees;
	Trees.SetNum(Size.X);
	for (int32 X = 0; X < Size.X; ++X)
	{
		Trees[X] = TArray<TSharedPtr<Tree>>();
		Trees[X].SetNum(Size.Y);
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			Trees[X][Y] = MakeShared<Tree>();
		}
	}

	TArray<FTreeEdge> Edges;
	Edges.Reserve((Size.X - 1) * (Size.Y - 1));
	for (int32 X = 0; X < Size.X; ++X)
	{
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			if (X > 0)
			{
				Edges.Emplace(X, Y, EDirection::West);
			}
			if (Y > 0)
			{
				Edges.Emplace(X, Y, EDirection::North);
			}
		}
	}

	const FRandomStream RandomStream(Seed);
	ShuffleTArray(Edges, RandomStream);

	while (!Edges.IsEmpty())
	{
		const FTreeEdge CurrentEdge = Edges.Pop();
		const TSharedPtr<Tree> CurrentTree = Trees[CurrentEdge.X][CurrentEdge.Y];
		int32 NextX = 0;
		int32 NextY = 0;
		EDirection OppositeDirection = EDirection::None;
		switch (CurrentEdge.Direction)
		{
		case EDirection::West:
			NextX = CurrentEdge.X - 1;
			NextY = CurrentEdge.Y;
			OppositeDirection = EDirection::East;
			break;
		case EDirection::East:
			NextX = CurrentEdge.X + 1;
			NextY = CurrentEdge.Y;
			OppositeDirection = EDirection::West;
			break;
		case EDirection::North:
			NextX = CurrentEdge.X;
			NextY = CurrentEdge.Y - 1;
			OppositeDirection = EDirection::South;
			break;
		case EDirection::South:
			NextX = CurrentEdge.X;
			NextY = CurrentEdge.Y + 1;
			OppositeDirection = EDirection::North;
			break;
		default:
			break;
		}
		const TSharedPtr<Tree> NextTree = Trees[NextX][NextY];
		if (!CurrentTree->IsConnected(NextTree.Get()))
		{
			CurrentTree->Connect(NextTree.Get());
			Grid[CurrentEdge.X][CurrentEdge.Y] |= static_cast<int32>(CurrentEdge.Direction);
			Grid[NextX][NextY] |= static_cast<int32>(OppositeDirection);
		}
	}

	return Grid;
}
