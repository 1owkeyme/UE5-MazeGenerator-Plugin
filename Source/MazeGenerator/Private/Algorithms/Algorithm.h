// Copyright LowkeyMe. All Rights Reserved. 2022

#pragma once

#include "CoreMinimal.h"

#include "Math/RandomStream.h"


enum class EDirection : uint8
{
	None = 0,
	East = 1,
	North = 2,
	South = 4,
	West = 8,
};

EDirection OppositeDirection(const EDirection Direction);

int32 DirectionDX(const EDirection Direction);
int32 DirectionDY(const EDirection Direction);

class Algorithm
{
public:
	virtual ~Algorithm() = default;

	TArray<TArray<uint8>> GetGrid(const FIntVector2& Size, const int32 Seed);

protected:
	static TArray<TArray<uint8>> CreateZeroedGrid(const FIntVector2& Size);

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) = 0;
};
