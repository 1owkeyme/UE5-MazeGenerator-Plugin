// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class Algorithm
{
public:
	virtual ~Algorithm() = default;

	virtual TArray<TArray<uint8>> GetGrid(const FVector2D& Size) = 0;
};
