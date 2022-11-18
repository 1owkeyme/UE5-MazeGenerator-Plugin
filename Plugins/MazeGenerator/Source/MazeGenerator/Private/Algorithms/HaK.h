// Copyright LowkeyMe. All Rights Reserved. 2022

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"


class HaK : public Algorithm
{
public:
	virtual ~HaK() override = default;

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) override;
	static TPair<int32, int32> Walk(TArray<TArray<uint8>>& Grid,
	                                const int32 X, const int32 Y,
	                                const FRandomStream& RandomStream);

	static TPair<int32, int32> Hunt(TArray<TArray<uint8>>& Grid, const FRandomStream& RandomStream);

	static TArray<EDirection> GetNeighbours(const int32 X, const int32 Y, const TArray<TArray<uint8>>& Grid);
};
