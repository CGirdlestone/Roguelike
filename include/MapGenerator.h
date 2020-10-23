#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <cstdint>
#include <vector>
#include <utility>

class MapGenerator
{
public:
	MapGenerator(int width, int height);
	~MapGenerator();

	char* generateCaveMap(const int threshold, const int steps, const int underPop, const int overPop);
	char* generateRoomMap(const int minRooms, const int maxRooms, const int minRoomWidth, const int maxRoomWidth);

	char* getLevel() const { return level; };
private:
	// variables
	int mapWidth;
	int mapHeight;

	char* level;

	// Cave Maps - Cellular Automata
	void initialiseCaveMap(const int threshold);
	void simulationStep(const int underPop, const int overPop);
	std::pair<int, int> floodFill();
	void connectCaves(std::pair<int, int> letterCodes);
	void smoothMap(const int minWallThreshold);
	void fillBorder();
	std::vector<int> getNeighbours(int i);
	int getNeighbourWallCount(int i);
	void hollowSolidChunks();

	// Room Maps - Position Rectangles
};

#endif
