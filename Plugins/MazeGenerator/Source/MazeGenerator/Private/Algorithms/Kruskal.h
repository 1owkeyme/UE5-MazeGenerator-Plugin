// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"


enum class EDirection : uint8
{
	None = 0,
	East = 1,
	North = 2,
	South = 4,
	West = 8,
};

struct FTreeEdge
{
	int32 X;
	int32 Y;
	EDirection Direction;

	FTreeEdge(const int32 X, const int32 Y, const EDirection Direction): X(X), Y(Y), Direction(Direction)
	{
	};
};


class Tree
{
	Tree* Parent;

public:
	Tree(): Parent(nullptr)
	{
	};

	Tree* GetRoot();

	bool IsConnected(Tree* const AnotherTree);

	void Connect(Tree* const TreeToConnect);
};

class Kruskal : public Algorithm
{
public:
	virtual TArray<TArray<uint8>> GetGrid(const FIntVector2& Size, const int32 Seed) override;

	virtual ~Kruskal() override = default;
private:
	TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size, const int32 Seed) const;
};
