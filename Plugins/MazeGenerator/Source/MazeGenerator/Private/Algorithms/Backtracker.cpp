// Copyright LowkeyMe. All Rights Reserved. 2022

#include "Backtracker.h"

#include "Utils.h"

TArray<TArray<uint8>> Backtracker::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	CarvePassagesFrom(0, 0, Grid, RandomStream);

	return Grid;
}

void Backtracker::CarvePassagesFrom(const int32 X, const int32 Y, TArray<TArray<uint8>>& Grid,
                                    const FRandomStream& RandomStream)
{
	const TArray<EDirection> Directions = ShuffleTArray<EDirection>(
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
			CarvePassagesFrom(NextX, NextY, Grid, RandomStream);
		}
	}
}
