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

	operator FIntVector2() const
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

	bool operator==(const FMazeCoordinates& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FMazeCoordinates& Other) const
	{
		return !(*this == Other);
	}

	operator TPair<int32, int32>() const
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

USTRUCT()
struct FMazePath
{
	GENERATED_BODY()

	TArray<TArray<uint8>> Grid;

	UPROPERTY(VisibleAnywhere)
	int32 Length;

	FMazePath(): Length(0)
	{
	}
};

class Algorithm;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class MAZEGENERATOR_API AMaze : public AActor
{
	GENERATED_BODY()

public:
	AMaze();

	UPROPERTY(EditAnywhere, Category="Maze", meta=(NoResetToDefault, DisplayPriority=0))
	EGenerationAlgorithm GenerationAlgorithm;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(DisplayPriority=1))
	uint64 Seed = 0;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(DisplayPriority=2))
	FMazeSize MazeSize;

	TArray<TArray<uint8>> MazeGrid;

	UPROPERTY(EditAnywhere, DisplayName="Floor", Category="Maze|Cells", meta=(NoResetToDefault, DisplayPriority=0))
	FMazeCell FloorCell;

	UPROPERTY(EditAnywhere, DisplayName="Wall", Category="Maze|Cells", meta=(NoResetToDefault, DisplayPriority=1))
	FMazeCell WallCell;

	UPROPERTY(EditAnywhere, DisplayName="Outline Wall", Category="Maze|Cells", meta=(DisplayPriority=2))
	FMazeCell OutlineWallCell;

	UPROPERTY(VisibleAnywhere, Category="Maze|Cells")
	FVector2D MazeCellSize;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder")
	bool bShowPath = true;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bShowPath", EditConditionHides))
	FMazeCoordinates PathStart;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bShowPath", EditConditionHides))
	FMazeCoordinates PathEnd;

	UPROPERTY(EditAnywhere, DisplayName="Path Floor", Category="Maze|Pathfinder",
		meta=(EditCondition="bShowPath", EditConditionHides, NoResetToDefault))
	FMazeCell PathFloorCell;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder",
		meta=(EditCondition="bShowPath", EditConditionHides, ShowOnlyInnerProperties))
	FMazePath MazePath;

private:
#if WITH_EDITOR
	FTransform LastMazeTransform;
#endif

	TMap<EGenerationAlgorithm, TSharedPtr<Algorithm>> GenerationAlgorithms;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* FloorCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* WallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* OutlineWallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PathFloorCells;

	void UpdateMaze();

	void GenerateMazeOutline() const;

	FMazePath GetMazePath(const FMazeCoordinates& Start, const FMazeCoordinates& End);

	void ClearMaze() const;


protected:
	virtual void BeginPlay() override;

public:
	// Remember: this method is being called before BeginPlay. 
	virtual void OnConstruction(const FTransform& Transform) override;
};
