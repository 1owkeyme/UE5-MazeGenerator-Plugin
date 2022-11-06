// Copyright LowkeyMe. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"

#include "Maze.generated.h"

class Algorithm;
class UHierarchicalInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class EGenerationAlgorithm : uint8
{
	Kruskal,
	Prim,
};

USTRUCT()
struct FMazeSize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 X;

	UPROPERTY(EditAnywhere)
	int32 Y;

	FMazeSize(): X(20), Y(20)
	{
	}

	explicit operator FVector2D() const
	{
		return FVector2D(X, Y);
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
	FMazeSize MazeSize;

	UPROPERTY(EditAnywhere, Category="Maze|Cells")
	FMazeCell FloorCell;

	UPROPERTY(EditAnywhere, Category="Maze|Cells")
	FMazeCell WallCell;

private:
	TMap<EGenerationAlgorithm, TSharedPtr<Algorithm>> GenerationAlgorithms;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* FloorCells;

	UPROPERTY()
	UHierarchicalInstancedStaticMeshComponent* WallCells;

	void GenerateMaze();

	void ClearMaze() const;
protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	virtual ~AMaze() override = default;
};
