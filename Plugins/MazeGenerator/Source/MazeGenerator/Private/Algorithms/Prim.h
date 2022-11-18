// Copyright LowkeyMe. All Rights Reserved. 2022

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"


enum class ECellState : uint8
{
	// It is important to have following values more than maximum of EDirection enum.
	None = 0,
	In = 64,
	Frontier = 128,
};

class Prim : public Algorithm
{
public:
	virtual ~Prim() override = default;

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) override;

	void ExpandFrontierFrom(const int32 X, const int32 Y, TArray<TArray<uint8>>& Grid);

	void ExpandFrontierWith(const int32 X, const int32 Y, TArray<TArray<uint8>>& Grid);

	static TArray<TPair<int32, int32>> GetNeighbours(const int32 X, const int32 Y, const TArray<TArray<uint8>>& Grid);

	static EDirection GetDirection(const TPair<int32, int32>& SourceCell, const TPair<int32, int32>& DestinationCell);

	TArray<TPair<int32, int32>> Frontier;
};
