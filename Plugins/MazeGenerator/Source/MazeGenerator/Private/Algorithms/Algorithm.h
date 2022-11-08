// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class Algorithm
{
public:
	virtual ~Algorithm() = default;

	virtual TArray<TArray<uint8>> GetGrid(const FIntVector2& Size, const int32 Seed) = 0;
};
