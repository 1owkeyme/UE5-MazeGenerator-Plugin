// Copyright LowkeyMe. All Rights Reserved. 2022

#include "HaK.h"

#include "Utils.h"

TArray<TArray<uint8>> HaK::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	const int32 RandomX = RandomStream.RandRange(0, Size.X - 1);
	const int32 RandomY = RandomStream.RandRange(0, Size.Y - 1);

	TPair<int32, int32> Cell(RandomX, RandomY);
	do
	{
		Cell = Walk(Grid, Cell.Key, Cell.Value, RandomStream);
		if (Cell.Key == -1)
		{
			Cell = Hunt(Grid, RandomStream);
		}
	}
	while (Cell.Key != -1);

	return Grid;
}

TPair<int32, int32> HaK::Walk(TArray<TArray<uint8>>& Grid,
                              const int32 X, const int32 Y,
                              const FRandomStream& RandomStream)
{
	const TArray<EDirection> Directions = ShuffleTArray(
		TArray{
			EDirection::West, EDirection::East,
			EDirection::North, EDirection::South
		}, RandomStream);

	for (int i = 0; i < Directions.Num(); ++i)
	{
		const EDirection Direction = Directions[i];
		const int32 NextX = X + DirectionDX(Direction);
		const int32 NextY = Y + DirectionDY(Direction);
		const bool bInBounds = NextY >= 0 && NextX >= 0 && NextY < Grid.Num() && NextX < Grid[Y].Num();
		if (bInBounds && Grid[NextY][NextX] == 0)
		{
			Grid[Y][X] |= static_cast<uint8>(Direction);
			Grid[NextY][NextX] |= static_cast<uint8>(OppositeDirection(Direction));
			return TPair<int32, int32>(NextX, NextY);
		}
	}
	return TPair<int32, int32>(-1, -1);
}

TPair<int32, int32> HaK::Hunt(TArray<TArray<uint8>>& Grid, const FRandomStream& RandomStream)
{
	for (int32 Y = 0; Y < Grid.Num(); ++Y)
	{
		for (int32 X = 0; X < Grid[Y].Num(); ++X)
		{
			if (Grid[Y][X] != 0)
			{
				continue;
			}
			const TArray<EDirection> PossibleDirections = GetNeighbours(X, Y, Grid);
			if (PossibleDirections.Num() == 0)
			{
				continue;
			}
			const EDirection ConnectDirection = PossibleDirections[RandomStream.RandRange(
				0, PossibleDirections.Num() - 1)];
			const int32 NextX = X + DirectionDX(ConnectDirection);
			const int32 NextY = Y + DirectionDY(ConnectDirection);

			Grid[Y][X] |= static_cast<uint8>(ConnectDirection);
			Grid[NextY][NextX] |= static_cast<uint8>(OppositeDirection(ConnectDirection));

			return TPair<int32, int32>(X, Y);
		}
	}
	return TPair<int32, int32>(-1, -1);
}

TArray<EDirection> HaK::GetNeighbours(const int32 X, const int32 Y, const TArray<TArray<uint8>>& Grid)
{
	TArray<EDirection> Directions;
	if (X > 0 && Grid[Y][X - 1])
	{
		Directions.Emplace(EDirection::West);
	}
	if (X + 1 < Grid[Y].Num() && Grid[Y][X + 1])
	{
		Directions.Emplace(EDirection::East);
	}
	if (Y > 0 && Grid[Y - 1][X])
	{
		Directions.Emplace(EDirection::North);
	}
	if (Y + 1 < Grid.Num() && Grid[Y + 1][X])
	{
		Directions.Emplace(EDirection::South);
	}
	return Directions;
}
