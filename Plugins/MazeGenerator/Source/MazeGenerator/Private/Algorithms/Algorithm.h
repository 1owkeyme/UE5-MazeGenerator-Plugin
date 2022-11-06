// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

typedef TArray<TArray<int8>> FGrid;


class Algorithm
{
public:
	virtual ~Algorithm() = default;

	virtual FGrid GetGrid(const FVector2D& Size) = 0;
};
