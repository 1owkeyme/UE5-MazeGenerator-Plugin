// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"

#include "Maze.generated.h"

UENUM(BlueprintType)
enum class EGenerationAlgorithm : uint8
{
	Backtracker UMETA(DisplayName="Recursive Backtracker"),
	Division UMETA(DisplayName="Recursive Division"),
	HaK UMETA(DisplayName="Hunt-and-Kill"),
	Sidewinder,
	Kruskal,
	Eller,
	Prim
};


USTRUCT()
struct FMazeSize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(ClampMin=3, UIMin=5, UIMax=101, ClampMax=9999, NoResetToDefault))
	int32 X;

	UPROPERTY(EditAnywhere, meta=(ClampMin=3, UIMin=5, UIMax=101, ClampMax=9999, NoResetToDefault))
	int32 Y;

	FMazeSize(): X(5), Y(5)
	{
	}

	explicit operator FIntVector2() const
	{
		return FIntVector2(X, Y);
	}
};


USTRUCT()
struct FMazeCoordinates
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(ClampMin=0, Delta=1, NoResetToDefault))
	int32 X;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0, Delta=1, NoResetToDefault))
	int32 Y;

	FMazeCoordinates(): X(0), Y(0)
	{
	}

	void ClampByMazeSize(const FMazeSize& MazeSize);

	explicit operator TPair<int32, int32>() const
	{
		return TPair<int32, int32>(X, Y);
	}
};


USTRUCT()
struct FMazeCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(NoResetToDefault))
	UStaticMesh* StaticMesh;

	void CalculateSize()
	{
		if (StaticMesh)
		{
			const FVector StaticMeshSize = StaticMesh->GetBoundingBox().GetSize();
			Size.X = StaticMeshSize.X;
			Size.Y = StaticMeshSize.Y;
		}
	}

	FVector2D GetSize()
	{
		if (Size.IsZero())
		{
			CalculateSize();
		}
		return Size;
	}

private:
	FVector2D Size;

public:
	explicit operator bool() const
	{
		return StaticMesh != nullptr;
	}
};


class Algorithm;
class Pathfinder;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class MAZEGENERATOR_API AMaze : public AActor
{
	GENERATED_BODY()

public:
	AMaze();

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category="Maze")
	bool bGenerateInEditor = true;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(NoResetToDefault, DisplayPriority=0))
	EGenerationAlgorithm GenerationAlgorithm;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(DisplayPriority=1))
	uint64 Seed = 0;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(DisplayPriority=2))
	FMazeSize MazeSize;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(NoResetToDefault))
	FMazeCell FloorCell;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(NoResetToDefault))
	FMazeCell WallCell;

	UPROPERTY(EditAnywhere, Category="Maze|Outline")
	bool bCreateOutline = true;

	UPROPERTY(EditAnywhere, Category="Maze|Outline", meta=(EditCondition="bCreateOutline", EditConditionHides))
	FMazeCell OutlineWallCell;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder")
	bool bEnablePathfinder = true;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bEnablePathfinder", EditConditionHides))
	FMazeCoordinates PathStart;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bEnablePathfinder", EditConditionHides))
	FMazeCoordinates PathEnd;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bEnablePathfinder", EditConditionHides, NoResetToDefault))
	FMazeCell PathFloorCell;


	TArray<TArray<uint8>> MazeGrid;

	TArray<TArray<uint8>> MazePathGrid;
private:
	FTransform LastMazeTransform;

	TMap<EGenerationAlgorithm, TSharedPtr<Algorithm>> GenerationAlgorithms;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* FloorCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* WallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* OutlineWallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PathFloorCells;


	void GenerateMaze(const bool bShouldUpdateMaze = true);

	void GenerateMazeOutline();

	void ClearMaze() const;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	virtual ~AMaze() override = default;
};
