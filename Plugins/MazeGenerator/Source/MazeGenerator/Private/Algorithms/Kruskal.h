// Copyright LowkeyMe. All Rights Reserved. 2022

#pragma once

#include "CoreMinimal.h"

#include "Algorithm.h"

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
public:
	Tree(): Parent(nullptr)
	{
	};

	Tree* GetRoot();

	bool IsConnected(Tree* const AnotherTree);

	void Connect(Tree* const TreeToConnect);

private:
	Tree* Parent;
};

class Kruskal : public Algorithm
{
public:
	virtual ~Kruskal() override = default;

private:
	virtual TArray<TArray<uint8>> GetDirectionsGrid(const FIntVector2& Size,
	                                                const FRandomStream& RandomStream) override;
};
