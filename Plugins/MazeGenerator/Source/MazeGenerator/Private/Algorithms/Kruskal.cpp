// Copyright LowkeyMe. All Rights Reserved.


#include "Kruskal.h"


TSharedPtr<Tree> Tree::GetRoot()
{
	return Parent ? Parent->GetRoot() : TSharedPtr<Tree>(this);
}

bool Tree::IsConnected(const TSharedPtr<Tree>& AnotherTree)
{
	return GetRoot() == AnotherTree->GetRoot();
}

void Tree::Connect(const TSharedPtr<Tree>& TreeToConnect)
{
	TreeToConnect->GetRoot()->Parent = TSharedPtr<Tree>(this);
}

TArray<TArray<uint8>> Kruskal::GetGrid(const FVector2D& Size)
{
	TArray<TArray<uint8>> Grid;
	Grid.Init(TArray<uint8>(), Size.X);
	for (int32 X = 0; X < Size.X; ++X)
	{
		Grid[X].SetNumZeroed(Size.Y);
	}

	TArray<TArray<TSharedPtr<Tree>>> Trees;
	Trees.Init(TArray<TSharedPtr<Tree>>(), Size.X);
	for (int32 X = 0; X < Size.X; ++X)
	{
		Trees[X].SetNum(Size.Y);
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			Trees[X][Y] = MakeShared<Tree>();
		}
	}

	TArray<FTreeEdge> Edges;
	Edges.Reserve((Size.X - 1) * (Size.Y - 1));
	for (int32 X = 1; X < Size.X; ++X)
	{
		for (int32 Y = 1; Y < Size.Y; ++Y)
		{
			Edges.Emplace(X, Y, EDirection::West);
			Edges.Emplace(X, Y, EDirection::North);
		}
	}

	while (!Edges.IsEmpty())
	{
		const FTreeEdge CurrentEdge = Edges.Pop();
		const TSharedPtr<Tree> CurrentTree = Trees[CurrentEdge.X][CurrentEdge.Y];
		int32 NextX = 0;
		int32 NextY = 0;
		EDirection OppositeDirection = EDirection::None;
		switch (CurrentEdge.Direction)
		{
		case EDirection::East:
			NextX = CurrentEdge.X + 1;
			OppositeDirection = EDirection::West;
			break;
		case EDirection::North:
			NextY = CurrentEdge.Y - 1;
			OppositeDirection = EDirection::South;
			break;
		case EDirection::South:
			NextY = CurrentEdge.Y + 1;
			OppositeDirection = EDirection::North;
			break;
		case EDirection::West:
			NextX = CurrentEdge.X - 1;
			OppositeDirection = EDirection::East;
			break;
		default:
			break;
		}
		const TSharedPtr<Tree> NextTree = Trees[NextX][NextY];
		if (CurrentTree->IsConnected(NextTree))
		{
			CurrentTree->Connect(NextTree);
			Grid[CurrentEdge.X][CurrentEdge.Y] |= CurrentEdge.Direction;
			Grid[NextX][NextY] |= OppositeDirection;
		}
		return Grid;
	}
}
