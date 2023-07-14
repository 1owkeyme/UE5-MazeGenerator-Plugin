// Copyright LowkeyMe. All Rights Reserved. 2022

#include "Sidewinder.h"

TArray<TArray<uint8>> Sidewinder::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	for (int Y = 0; Y < Size.Y; ++Y)
	{
		int32 RunStart = 0;
		for (int X = 0; X < Size.X; ++X)
		{
			if (Y > 0 && (X + 1 == Size.X || RandomStream.RandRange(0, 1)))
			{
				const int32 PassageCellX = RunStart + RandomStream.RandRange(0, X - RunStart);
				Grid[Y][PassageCellX] |= static_cast<uint8>(EDirection::North);
				Grid[Y - 1][PassageCellX] |= static_cast<uint8>(EDirection::South);
				RunStart = X + 1;
			}
			else if (X + 1 < Size.X)
			{
				Grid[Y][X] |= static_cast<uint8>(EDirection::East);
				Grid[Y][X + 1] |= static_cast<uint8>(EDirection::West);
			}
		}
	}

	return Grid;
}
