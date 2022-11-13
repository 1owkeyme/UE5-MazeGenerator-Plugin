#pragma once

class Pathfinder
{
public:
	TArray<TArray<uint8>> FindPath(const TArray<TArray<uint8>>& Maze,
	                               const TPair<int32, int32>& Start, const TPair<int32, int32>& End);
};
