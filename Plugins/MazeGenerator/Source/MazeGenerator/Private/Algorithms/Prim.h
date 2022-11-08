// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"


class Prim : public Algorithm
{
public:
	virtual TArray<TArray<uint8>> GetGrid(const FIntVector2& Size, const int32 Seed) override;

	virtual ~Prim() override = default;
};
