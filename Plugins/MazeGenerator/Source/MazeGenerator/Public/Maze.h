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

	FVector2D GetSize()
	{
		if (StaticMesh)
		{
			const FVector StaticMeshSize = StaticMesh->GetBoundingBox().GetSize();
			Size.X = StaticMeshSize.X;
			Size.Y = StaticMeshSize.Y;
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

	UPROPERTY(EditAnywhere, DisplayName="Floor", Category="Maze|Cells", meta=(NoResetToDefault, DisplayPriority=0))
	FMazeCell FloorCell;

	UPROPERTY(EditAnywhere, DisplayName="Wall", Category="Maze|Cells", meta=(NoResetToDefault, DisplayPriority=1))
	FMazeCell WallCell;

	UPROPERTY(EditAnywhere, DisplayName="OutlineWall", Category="Maze|Cells", meta=(DisplayPriority=2))
	FMazeCell OutlineWallCell;

	UPROPERTY(VisibleAnywhere, Category="Maze|Cells")
	FVector2D MazeCellSize;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder")
	bool bEnablePathfinder = true;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bEnablePathfinder", EditConditionHides))
	FMazeCoordinates PathStart;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bEnablePathfinder", EditConditionHides))
	FMazeCoordinates PathEnd;

	UPROPERTY(EditAnywhere, DisplayName="PathFloor", Category="Maze|Pathfinder",
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

	void GenerateMazeOutline() const;

	void ClearMaze() const;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnConstruction(const FTransform& Transform) override;
};
