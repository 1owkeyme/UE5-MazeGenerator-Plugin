// Copyright LowkeyMe. All Rights Reserved. 2022

#include "Algorithm.h"

EDirection OppositeDirection(const EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::East:
		return EDirection::West;
	case EDirection::West:
		return EDirection::East;
	case EDirection::South:
		return EDirection::North;
	case EDirection::North:
		return EDirection::South;
	default:
		return EDirection::None;
	}
}

int32 DirectionDX(const EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::East:
		return 1;
	case EDirection::West:
		return -1;
	default:
		return 0;
	}
}

int32 DirectionDY(const EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::North:
		return -1;
	case EDirection::South:
		return 1;
	default:
		return 0;
	}
}

TArray<TArray<uint8>> Algorithm::GetGrid(const FIntVector2& Size, const int32 Seed)
{
	// There is for each 2 not connected floors 1 wall between.
	const FIntVector2 DirectionsGridSize((Size.X + 1) / 2, (Size.Y + 1) / 2);

	const FRandomStream RandomStream(Seed);

	const TArray<TArray<uint8>> DirectionsGrid = GetDirectionsGrid(DirectionsGridSize, RandomStream);

	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

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

TArray<TArray<uint8>> Algorithm::CreateZeroedGrid(const FIntVector2& Size)
{
	TArray<TArray<uint8>> Grid;
	Grid.Init(TArray<uint8>(), Size.Y);
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		Grid[Y].SetNumZeroed(Size.X);
	}
	return Grid;
}	
