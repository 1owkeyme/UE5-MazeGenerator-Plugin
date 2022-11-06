// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"


class Kruskal : public Algorithm
{
public:
	virtual FGrid GetGrid(const FVector2D& Size) override;

	virtual ~Kruskal() override = default;
};
