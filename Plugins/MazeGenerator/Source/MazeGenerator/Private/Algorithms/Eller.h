// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"


class Eller : public Algorithm
{
public:
	virtual ~Eller() override = default;

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) override;
};
