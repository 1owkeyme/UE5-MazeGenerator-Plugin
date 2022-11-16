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
		return FIntVector2{X, Y};
	}
};

USTRUCT(BlueprintType)
struct FMazeCoordinates
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoSpinbox=true, ClampMin=0, NoResetToDefault))
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(NoSpinbox=true, ClampMin=0, Delta=1, NoResetToDefault))
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
		return TPair<int32, int32>{X, Y};
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
	int32 Seed;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder", meta=(ExposeOnSpawn))
	bool bGeneratePath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder",
		meta=(ExposeOnSpawn, EditCondition="bGeneratePath", EditConditionHides))
	FMazeCoordinates PathStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maze|Pathfinder",
		meta=(ExposeOnSpawn, EditCondition="bGeneratePath", EditConditionHides))
	FMazeCoordinates PathEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Path Floor", Category="Maze|Pathfinder",
		meta=(ExposeOnSpawn, EditCondition="bGeneratePath", EditConditionHides))
	UStaticMesh* PathStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Maze|Pathfinder",
		meta=(EditCondition="bGeneratePath", EditConditionHides))
	int32 PathLength;

protected:
	// Unfortunately, UE reflection system doesn't support 2-dimensional arrays.
	TArray<TArray<uint8>> MazeGrid;

	TArray<TArray<uint8>> MazePathGrid;

	TMap<EGenerationAlgorithm, TSharedPtr<Algorithm>> GenerationAlgorithms;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* FloorCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* WallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* OutlineWallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PathFloorCells;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Maze|Cells")
	FVector2D MazeCellSize;

public:
	// Update Maze according to pre-set parameters: Size, Generation Algorithm, Seed and Path-related params.
	UFUNCTION(BlueprintCallable)
	virtual void UpdateMaze();

	/** Updates Maze every time any parameter has been changed(except transform).
	 *
	 * Remember: this method is being called before BeginPlay. 
	 */
	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	// Generate Maze with random size, seed and algorithm
	// with path connecting top-left and bottom-right corners.
	UFUNCTION(CallInEditor, Category="Maze", meta=(DisplayPriority=0, ShortTooltip = "Generate an arbitrary maze."))
	virtual void Randomize();

	virtual void CreateMazeOutline() const;

	/** Returns path grid mapped into MazeGrid constrains. Creates a graph every time it is called.
	 *
	 * Note :
	 * 
	 * Optimization is possible:
	 * if the beginning or end has not changed, there is actually no need to create a new graph,
	 * but due to the many parameters that can be changed, it is difficult to determine what exactly has changed,
	 * so this optimization has been neglected.
	 */
	virtual TArray<TArray<uint8>> GetMazePath(const FMazeCoordinates& Start, const FMazeCoordinates& End,
	                                          int32& OutLength);

	// Clears all HISM instances.
	virtual void ClearMaze() const;

	virtual FVector2D GetMaxCellSize() const;


#if WITH_EDITOR
private:
	FTransform LastMazeTransform;
#endif
};
