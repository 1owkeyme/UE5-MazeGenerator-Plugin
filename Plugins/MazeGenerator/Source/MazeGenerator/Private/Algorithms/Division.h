// Copyright LowkeyMe. All Rights Reserved. 2022

#pragma once

#include "Algorithm.h"


enum class EDivisionOrientation: uint8
{
	None,
	Horizontal,
	Vertical
};


class Division : public Algorithm
{
public:
	virtual ~Division() override = default;

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) override;

	void Divide(TArray<TArray<uint8>>& Grid,
	            const int32 X, const int32 Y,
	            const FIntVector2& Size,
	            const FRandomStream& RandomStream,
	            EDivisionOrientation Orientation);

	static EDivisionOrientation ChooseOrientation(const FIntVector2& Size, const FRandomStream& RandomStream);
};
