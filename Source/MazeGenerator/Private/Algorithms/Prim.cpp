// Copyright LowkeyMe. All Rights Reserved. 2022


#include "Prim.h"

TArray<TArray<uint8>> Prim::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	const int32 RandomX = RandomStream.RandRange(0, Size.X - 1);
	const int32 RandomY = RandomStream.RandRange(0, Size.Y - 1);

	ExpandFrontierFrom(RandomX, RandomY, Grid);

	while (!Frontier.IsEmpty())
	{
		const int32 Index = RandomStream.RandRange(0, Frontier.Num() - 1);
		const TPair<int32, int32> CurrentCell = Frontier[Index];
		Frontier.RemoveAt(Index);

		TArray<TPair<int32, int32>> Neighbours = GetNeighbours(CurrentCell.Key, CurrentCell.Value, Grid);
		const TPair<int32, int32> NextCell = Neighbours[RandomStream.RandRange(0, Neighbours.Num() - 1)];

		EDirection Direction = GetDirection(CurrentCell, NextCell);

		// Grid[Y][X] = ...
		Grid[CurrentCell.Value][CurrentCell.Key] |= static_cast<uint8>(Direction);
		Grid[NextCell.Value][NextCell.Key] |= static_cast<uint8>(OppositeDirection(Direction));

		ExpandFrontierFrom(CurrentCell.Key, CurrentCell.Value, Grid);
	}

	return Grid;
}

void Prim::ExpandFrontierFrom(const int32 X, const int32 Y, TArray<TArray<uint8>>& Grid)
{
	Grid[Y][X] |= static_cast<uint8>(ECellState::In);
	ExpandFrontierWith(X - 1, Y, Grid);
	ExpandFrontierWith(X + 1, Y, Grid);
	ExpandFrontierWith(X, Y + 1, Grid);
	ExpandFrontierWith(X, Y - 1, Grid);
}

void Prim::ExpandFrontierWith(const int32 X, const int32 Y, TArray<TArray<uint8>>& Grid)
{
	const bool bInBounds = Y >= 0 && X >= 0 && Y < Grid.Num() && X < Grid[Y].Num();
	if (bInBounds && Grid[Y][X] == 0)
	{
		Grid[Y][X] |= static_cast<uint8>(ECellState::Frontier);
		Frontier.Emplace(X, Y);
	}
}

TArray<TPair<int32, int32>> Prim::GetNeighbours(const int32 X, const int32 Y, const TArray<TArray<uint8>>& Grid)
{
	TArray<TPair<int32, int32>> Neighbours;
	if (X > 0 && Grid[Y][X - 1] & static_cast<uint8>(ECellState::In))
	{
		Neighbours.Emplace(X - 1, Y);
	}
	if (X + 1 < Grid[Y].Num() && Grid[Y][X + 1] & static_cast<uint8>(ECellState::In))
	{
		Neighbours.Emplace(X + 1, Y);
	}
	if (Y > 0 && Grid[Y - 1][X] & static_cast<uint8>(ECellState::In))
	{
		Neighbours.Emplace(X, Y - 1);
	}
	if (Y + 1 < Grid.Num() && Grid[Y + 1][X] & static_cast<uint8>(ECellState::In))
	{
		Neighbours.Emplace(X, Y + 1);
	}
	return Neighbours;
}

EDirection Prim::GetDirection(const TPair<int32, int32>& SourceCell, const TPair<int32, int32>& DestinationCell)
{
	if (SourceCell.Key < DestinationCell.Key)
	{
		return EDirection::East;
	}
	if (SourceCell.Key > DestinationCell.Key)
	{
		return EDirection::West;
	}
	if (SourceCell.Value < DestinationCell.Value)
	{
		return EDirection::South;
	}
	if (SourceCell.Value > DestinationCell.Value)
	{
		return EDirection::North;
	}

	return EDirection::None;
}
