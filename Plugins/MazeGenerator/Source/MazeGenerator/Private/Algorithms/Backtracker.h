// Copyright LowkeyMe. All Rights Reserved. 2022

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"

class Backtracker : public Algorithm
{
public:
	virtual ~Backtracker() override = default;

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) override;

	void CarvePassagesFrom(const int32 X, const int32 Y, TArray<TArray<uint8>>& Grid,
	                       const FRandomStream& RandomStream);
};
