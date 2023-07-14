// Copyright LowkeyMe. All Rights Reserved. 2022

#include "Eller.h"

TArray<TArray<uint8>> Eller::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	TArray<uint32> Row;
	Row.SetNumZeroed(Size.X);

	uint32 SetsCounter = 0;

	// Initialize all rows except last one.
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			if (!Row[X])
			{
				Row[X] = ++SetsCounter;
			}
		}

		if (Y == Size.Y - 1)
		{
			break;
		}

		for (int32 X = 0; X < Size.X - 1; ++X)
		{
			if (Row[X] != Row[X + 1] && !RandomStream.RandRange(0, 1))
			{
				Grid[Y][X] |= static_cast<uint8>(EDirection::East);
				Grid[Y][X + 1] |= static_cast<uint8>(EDirection::West);

				const uint32 DissolvedSet = Row[X + 1];
				do
				{
					Row[X + 1] = Row[X];
					X++;
				}
				while (X < Size.X - 1 && Row[X + 1] == DissolvedSet);
			}
		}

		// Create vertical passages.
		for (int32 PassagesCount = 0, CurrentSet,
		           CellsAmount = 1, // For current set.
		           X = 0; X < Size.X; ++X, ++CellsAmount)
		{
			CurrentSet = Row[X];
			if (RandomStream.RandRange(0, 1))
			{
				++PassagesCount;

				Grid[Y][X] |= static_cast<uint8>(EDirection::South);
				Grid[Y + 1][X] |= static_cast<uint8>(EDirection::North);
			}
			else
			{
				Row[X] = 0;
			}

			if (X == Size.X - 1 // If last cell. 
				|| CurrentSet != Row[X + 1]) // If set is about to change.
			{
				//Ensure there at least one vertical passage in previous set.
				if (!PassagesCount)
				{
					const int32 RandomX = RandomStream.RandRange(X - CellsAmount + 1, X);

					Row[RandomX] = CurrentSet;

					Grid[Y][RandomX] |= static_cast<uint8>(EDirection::South);
					Grid[Y + 1][RandomX] |= static_cast<uint8>(EDirection::North);
				}


				PassagesCount = 0;
				CellsAmount = 0;
			}
		}
	}

	// Create last row.
	for (int X = 0; X < Size.X - 1; ++X)
	{
		if (Row[X] != Row[X + 1])
		{
			Grid[Size.Y - 1][X] |= static_cast<uint8>(EDirection::East);
			Grid[Size.Y - 1][X + 1] |= static_cast<uint8>(EDirection::West);

			const uint32 DissolvedSet = Row[X + 1];
			do
			{
				Row[X + 1] = Row[X];
				X++;
			}
			while (X < Size.X - 1 && Row[X + 1] == DissolvedSet);

			// After this loop, X is the index of the last merged element,
			// and then the outer loop will increment X, so it is needed to decrement once 
			--X;
		}
	}

	return Grid;
}
