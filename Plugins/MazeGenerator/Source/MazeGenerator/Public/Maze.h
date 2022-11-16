// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

USTRUCT(BlueprintType)
struct FMazeSize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=3, UIMin=5, UIMax=101, ClampMax=9999, NoResetToDefault))
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=3, UIMin=5, UIMax=101, ClampMax=9999, NoResetToDefault))
	int32 Y;

	FMazeSize(): X(5), Y(5)
	{
	}

	operator FIntVector2() const
	{
		return FIntVector2(X, Y);
	}
};

USTRUCT(BlueprintType)
struct FMazeCoordinates
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, Delta=1, NoResetToDefault))
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, Delta=1, NoResetToDefault))
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

USTRUCT(BlueprintType)
struct FMazePath
{
	GENERATED_BODY()

	TArray<TArray<uint8>> Grid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze",
		meta=(NoResetToDefault, ExposeOnSpawn, DisplayPriority=0))
	EGenerationAlgorithm GenerationAlgorithm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze", meta=(ExposeOnSpawn, DisplayPriority=1))
	int32 Seed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze", meta=(ExposeOnSpawn, DisplayPriority=2))
	FMazeSize MazeSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Floor", Category="Maze|Cells",
		meta=(NoResetToDefault, ExposeOnSpawn, DisplayPriority=0))
	UStaticMesh* FloorStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Wall", Category="Maze|Cells",
		meta=(NoResetToDefault, ExposeOnSpawn, DisplayPriority=1))
	UStaticMesh* WallStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Outline Wall", Category="Maze|Cells",
		meta=(ExposeOnSpawn, DisplayPriority=2))
	UStaticMesh* OutlineStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Maze|Cells")
	FVector2D MazeCellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder", meta=(ExposeOnSpawn))
	bool bShowPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder",
		meta=(ExposeOnSpawn, EditCondition="bShowPath", EditConditionHides))
	FMazeCoordinates PathStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder",
		meta=(ExposeOnSpawn, EditCondition="bShowPath", EditConditionHides))
	FMazeCoordinates PathEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Path Floor", Category="Maze|Pathfinder",
		meta=(ExposeOnSpawn, EditCondition="bShowPath", EditConditionHides))
	UStaticMesh* PathStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder",
		meta=(EditCondition="bShowPath", EditConditionHides, ShowOnlyInnerProperties))
	FMazePath MazePath;

protected:
	TArray<TArray<uint8>> MazeGrid;

	// Update Maze according to pre-set parameters: Size, Generation Algorithm, Seed and Path-related params.
	UFUNCTION(BlueprintCallable)
	virtual void UpdateMaze();

	// Generate Maze with random size, seed and algorithm
	// with path connecting top-left and bottom-right corners.
	UFUNCTION(CallInEditor, Category="Maze", meta=(DisplayPriority=0, ShortTooltip = "Generate an arbitrary maze."))
	virtual void Randomize();
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

	void CreateMazeOutline() const;

	FMazePath GetMazePath(const FMazeCoordinates& Start, const FMazeCoordinates& End);

	void ClearMaze() const;

	FVector2D GetMaxCellSize() const;
protected:
	virtual void BeginPlay() override;

public:
	// Remember: this method is being called before BeginPlay. 
	virtual void OnConstruction(const FTransform& Transform) override;
};
