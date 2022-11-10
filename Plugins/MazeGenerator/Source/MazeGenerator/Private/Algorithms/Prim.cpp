// Copyright LowkeyMe. All Rights Reserved.


#include "Prim.h"

TArray<TArray<uint8>> Prim::GetGrid(const FIntVector2& Size, const int32 Seed)
{
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

TArray<TArray<uint8>> Prim::GetDirectionsGrid(const FIntVector2& Size, const int32 Seed)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	const FRandomStream RandomStream(Seed);

	const int32 RandomWidth = RandomStream.RandRange(0, Size.X - 1);
	const int32 RandomHeight = RandomStream.RandRange(0, Size.Y - 1);

	ExpandFrontierFrom(RandomWidth, RandomHeight, Grid);

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
	const bool bInBounds = X >= 0 && Y >= 0 && X < Grid[0].Num() && Y < Grid.Num();
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
		const int32 x = X;
		const int32 y = Y;
		Neighbours.Emplace(x - 1, y);
	}
	if (X + 1 < Grid[Y].Num() && Grid[Y][X + 1] & static_cast<uint8>(ECellState::In))
	{
		const int32 x = X;
		const int32 y = Y;
		Neighbours.Emplace(x + 1, y);
	}
	if (Y > 0 && Grid[Y - 1][X] & static_cast<uint8>(ECellState::In))
	{
		const int32 x = X;
		const int32 y = Y;
		Neighbours.Emplace(x, y - 1);
	}
	if (Y + 1 < Grid.Num() && Grid[Y + 1][X] & static_cast<uint8>(ECellState::In))
	{
		const int32 x = X;
		const int32 y = Y;
		Neighbours.Emplace(x, y + 1);
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
