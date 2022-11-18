// Copyright LowkeyMe. All Rights Reserved. 2022


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

TArray<TArray<uint8>> Kruskal::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
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

	ShuffleTArray(Edges, RandomStream);

	while (!Edges.IsEmpty())
	{
		const FTreeEdge CurrentEdge = Edges.Pop();
		const TSharedPtr<Tree> CurrentTree = Trees[CurrentEdge.Y][CurrentEdge.X];
		const int32 NextX = CurrentEdge.X + DirectionDX(CurrentEdge.Direction);
		const int32 NextY = CurrentEdge.Y + DirectionDY(CurrentEdge.Direction);

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
