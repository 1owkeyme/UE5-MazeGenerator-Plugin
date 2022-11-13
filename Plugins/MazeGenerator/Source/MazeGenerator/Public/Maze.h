// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"

#include "Maze.generated.h"

class Algorithm;
class UHierarchicalInstancedStaticMeshComponent;

#define MAZE_MIN_SIZE 3
#define MAZE_MAX_SIZE 100

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
struct FIntVector2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(ClampMin=3, UIMin=5, UIMax=101, ClampMax=9999))
	int32 X;

	UPROPERTY(EditAnywhere, meta=(ClampMin=3, UIMin=5, UIMax=101, ClampMax=9999))
	int32 Y;

	FIntVector2D(): X(0), Y(0)
	{
	}

	explicit FIntVector2D(const int32 X, const int32 Y): X(X), Y(Y)
	{
	}

	explicit operator const FIntVector2() const
	{
		return FIntVector2(X, Y);
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

UCLASS()
class MAZEGENERATOR_API AMaze : public AActor
{
	GENERATED_BODY()

public:
	AMaze();

	UPROPERTY(EditAnywhere, Category="Maze")
	bool bGenerateInEditor = true;

	UPROPERTY(EditAnywhere, Category="Maze")
	uint64 Seed = 0;

	UPROPERTY(EditAnywhere, Category="Maze", meta=(NoResetToDefault))
	EGenerationAlgorithm GenerationAlgorithm;

	UPROPERTY(EditAnywhere, Category="Maze")
	FIntVector2D MazeSize = FIntVector2D(5, 5);

	UPROPERTY(EditAnywhere, Category="Maze|Cells", meta=(NoResetToDefault))
	FMazeCell FloorCell;

	UPROPERTY(EditAnywhere, Category="Maze|Cells", meta=(NoResetToDefault))
	FMazeCell WallCell;

	UPROPERTY(EditAnywhere, Category="Maze|Cells")
	FMazeCell OutlineWallCell;

	UPROPERTY(EditAnywhere, Category="Maze|Cells", meta=(EditCondition="bEnablePathfinder"))
	FMazeCell PathFloorCell;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder")
	bool bEnablePathfinder = true;

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder", meta=(NoSpinbox, EditCondition="bEnablePathfinder"))
	FIntVector2D Start = FIntVector2D(0, 0);

	UPROPERTY(EditAnywhere, Category="Maze|Pathfinder", meta=(NoSpinbox, EditCondition="bEnablePathfinder"))
	FIntVector2D End = FIntVector2D(0, 0);

private:
	TMap<EGenerationAlgorithm, TSharedPtr<Algorithm>> GenerationAlgorithms;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* FloorCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* WallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* OutlineWallCells;
	
	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* PathFloorCells;
	
	void GenerateMaze();

	void GenerateMazeOutline();

	void ClearMaze() const;

protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	virtual ~AMaze() override = default;
};
