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
