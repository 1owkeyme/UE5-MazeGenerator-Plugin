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

	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	// Remap directions grid onto regular grid.
	for (int32 Y = 0; Y < DirectionsGridSize.Y; ++Y)
	{
		for (int32 X = 0; X < DirectionsGridSize.X; ++X)
		{
			Grid[Y * 2][X * 2] = 1;

			//It only makes sense to check the western and northern directions,
			//because the remaining ones would simply overlap.

			if (DirectionsGrid[Y][X] & static_cast<uint8>(EDirection::West))
			{
				Grid[Y * 2][X * 2 - 1] = 1;
			}
			if (DirectionsGrid[Y][X] & static_cast<uint8>(EDirection::North))
			{
				Grid[Y * 2 - 1][X * 2] = 1;
			}
		}
	}

	return Grid;
}

TArray<TArray<uint8>> Kruskal::GetDirectionsGrid(const FIntVector2& Size, const int32 Seed) const
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	TArray<TArray<TSharedPtr<Tree>>> Trees;
	Trees.SetNum(Size.Y);
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		Trees[Y] = TArray<TSharedPtr<Tree>>();
		Trees[Y].SetNum(Size.X);
		for (int32 X = 0; X < Size.X; ++X)
		{
			Trees[Y][X] = MakeShared<Tree>();
		}
	}

	TArray<FTreeEdge> Edges;
	Edges.Reserve((Size.X - 1) * (Size.Y - 1));
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
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
		const TSharedPtr<Tree> CurrentTree = Trees[CurrentEdge.Y][CurrentEdge.X];
		int32 NextX = 0;
		int32 NextY = 0;
		switch (CurrentEdge.Direction)
		{
		case EDirection::West:
			NextX = CurrentEdge.X - 1;
			NextY = CurrentEdge.Y;
			break;
		case EDirection::East:
			NextX = CurrentEdge.X + 1;
			NextY = CurrentEdge.Y;
			break;
		case EDirection::North:
			NextX = CurrentEdge.X;
			NextY = CurrentEdge.Y - 1;
			break;
		case EDirection::South:
			NextX = CurrentEdge.X;
			NextY = CurrentEdge.Y + 1;
			break;
		default:
			break;
		}
		const TSharedPtr<Tree> NextTree = Trees[NextY][NextX];
		if (!CurrentTree->IsConnected(NextTree.Get()))
		{
			CurrentTree->Connect(NextTree.Get());
			Grid[CurrentEdge.Y][CurrentEdge.X] |= static_cast<int32>(CurrentEdge.Direction);
			Grid[NextY][NextX] |= static_cast<int32>(OppositeDirection(CurrentEdge.Direction));
		}
	}

	return Grid;
}
