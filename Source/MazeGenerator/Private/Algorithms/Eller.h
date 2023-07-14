// Copyright LowkeyMe. All Rights Reserved. 2022

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
