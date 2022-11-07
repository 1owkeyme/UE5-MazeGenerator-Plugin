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

	FTreeEdge(int32 X, int32 Y, EDirection Direction): X(X), Y(Y), Direction(Direction)
	{
	};
};


class Tree
{
	TSharedPtr<Tree> Parent;

public:
	Tree(): Parent(nullptr)
	{
	};

	TSharedPtr<Tree> GetRoot();

	bool IsConnected(const TSharedPtr<Tree>& AnotherTree);

	void Connect(const TSharedPtr<Tree>& TreeToConnect);
};

class Kruskal : public Algorithm
{
public:
	virtual TArray<TArray<uint8>> GetGrid(const FVector2D& Size) override;

	virtual ~Kruskal() override = default;
};
