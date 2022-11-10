// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


enum class EDirection : uint8
{
	None = 0,
	East = 1,
	North = 2,
	South = 4,
	West = 8,
};

EDirection OppositeDirection(const EDirection Direction);

class Algorithm
{
public:
	virtual ~Algorithm() = default;

	virtual TArray<TArray<uint8>> GetGrid(const FIntVector2& Size, const int32 Seed) = 0;

protected:
	static TArray<TArray<uint8>> CreateZeroedGrid(const FIntVector2& Size);
};
