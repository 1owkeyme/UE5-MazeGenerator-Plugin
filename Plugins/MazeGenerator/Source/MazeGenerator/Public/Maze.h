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
	Kruskal,
	Prim,
	Backtracker
};

USTRUCT()
struct FMazeSize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(ClampMin=3, ClampMax=200))
	int32 X;

	UPROPERTY(EditAnywhere, meta=(ClampMin=3, ClampMax=200))
	int32 Y;

	FMazeSize(): X(3), Y(3)
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

	UPROPERTY(EditAnywhere)
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

	UPROPERTY(EditAnywhere, Category="Maze")
	EGenerationAlgorithm GenerationAlgorithm;

	UPROPERTY(EditAnywhere, Category="Maze")
	bool bGenerateOutline = true;

	UPROPERTY(EditAnywhere, Category="Maze")
	FMazeSize MazeSize;

	UPROPERTY(EditAnywhere, Category="Maze|Cells")
	FMazeCell FloorCell;

	UPROPERTY(EditAnywhere, Category="Maze|Cells")
	FMazeCell WallCell;

	UPROPERTY(EditAnywhere, Category="Maze|Cells")
	FMazeCell OutlineWallCell;

private:
	TMap<EGenerationAlgorithm, TSharedPtr<Algorithm>> GenerationAlgorithms;


	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* FloorCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* WallCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* OutlineWallCells;

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
