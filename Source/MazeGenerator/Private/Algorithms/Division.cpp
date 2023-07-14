// Copyright LowkeyMe. All Rights Reserved. 2022

#include "Division.h"

TArray<TArray<uint8>> Division::GetDirectionsGrid(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	TArray<TArray<uint8>> Grid = CreateZeroedGrid(Size);

	Divide(Grid, 0, 0, Size, RandomStream, EDivisionOrientation::Horizontal);

	return Grid;
}

void Division::Divide(TArray<TArray<uint8>>& Grid,
                      const int32 X, const int32 Y,
                      const FIntVector2& Size,
                      const FRandomStream& RandomStream,
                      EDivisionOrientation Orientation)
{
	if (Size.X < 2 || Size.Y < 2)
	{
		return;
	}


	const bool bIsHorizontal = Orientation == EDivisionOrientation::Horizontal;
	const int32 Dx = bIsHorizontal ? 1 : 0;
	const int32 Dy = bIsHorizontal ? 0 : 1;

	int32 WallX = X + (bIsHorizontal ? 0 : RandomStream.RandRange(0, Size.X - 3));
	int32 WallY = Y + (bIsHorizontal ? RandomStream.RandRange(0, Size.Y - 3) : 0);
	const int32 PassX = WallX + (bIsHorizontal ? RandomStream.RandRange(0, Size.X - 1) : 0);
	const int32 PassY = WallY + (bIsHorizontal ? 0 : RandomStream.RandRange(0, Size.Y - 1));


	const int32 Length = bIsHorizontal ? Size.X : Size.Y;

	EDirection WallDirection = bIsHorizontal ? EDirection::East : EDirection::South;
	EDirection PassDirection = bIsHorizontal ? EDirection::South : EDirection::East;

	// Walk along the wall and indicate the directions for both sides of the wall.
	for (int32 i = 0; i < Length; ++i)
	{
		if (i < Length - 1)
		{
			Grid[WallY][WallX] |= static_cast<uint8>(WallDirection);
			Grid[WallY + Dy][WallX + Dx] |= static_cast<uint8>(OppositeDirection(WallDirection));

			Grid[WallY + Dx][WallX + Dy] |= static_cast<uint8>(WallDirection);
			Grid[WallY + Dx + Dy][WallX + Dx + Dy] |= static_cast<uint8>(OppositeDirection(WallDirection));
		}

		if (WallX == PassX && WallY == PassY)
		{
			Grid[WallY][WallX] |= static_cast<uint8>(PassDirection);
			Grid[WallY + Dx][WallX + Dy] |= static_cast<uint8>(OppositeDirection(PassDirection));
		}
		else
		{
			Grid[WallY][WallX] &= ~static_cast<uint8>(PassDirection);
			Grid[WallY + Dx][WallX + Dy] &= ~static_cast<uint8>(OppositeDirection(PassDirection));

			Grid[WallY][WallX] &= ~static_cast<uint8>(PassDirection);
			Grid[WallY + Dx][WallX + Dy] &= ~static_cast<uint8>(OppositeDirection(PassDirection));
		}


		WallX += Dx;
		WallY += Dy;
	}


	FIntVector2 NextSize;

	int32 NextX = X;
	int32 NextY = Y;
	NextSize.X = bIsHorizontal ? Size.X : WallX - X + 1;
	NextSize.Y = bIsHorizontal ? WallY - Y + 1 : Size.Y;

	Divide(Grid, NextX, NextY, NextSize, RandomStream, ChooseOrientation(NextSize, RandomStream));

	NextX = bIsHorizontal ? X : WallX + 1;
	NextY = bIsHorizontal ? WallY + 1 : Y;
	NextSize.X = bIsHorizontal ? Size.X : X + Size.X - WallX - 1;
	NextSize.Y = bIsHorizontal ? Y + Size.Y - WallY - 1 : Size.Y;

	Divide(Grid, NextX, NextY, NextSize, RandomStream, ChooseOrientation(NextSize, RandomStream));
}

EDivisionOrientation Division::ChooseOrientation(const FIntVector2& Size, const FRandomStream& RandomStream)
{
	if (Size.X < Size.Y)
	{
		return EDivisionOrientation::Horizontal;
	}
	if (Size.Y < Size.X)
	{
		return EDivisionOrientation::Vertical;
	}
	return RandomStream.RandRange(0, 1) ? EDivisionOrientation::Horizontal : EDivisionOrientation::Vertical;
}
