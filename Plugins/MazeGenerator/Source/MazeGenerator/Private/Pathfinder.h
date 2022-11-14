#pragma once

class Pathfinder
{
public:
	// Does not check whether the desired path falls within the size of the maze.
	static TArray<TArray<uint8>> FindPath(const TArray<TArray<uint8>>& Maze,
	                                      const TPair<int32, int32>& Start, const TPair<int32, int32>& End);
};
