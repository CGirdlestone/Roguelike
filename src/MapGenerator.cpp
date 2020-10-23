
#include "MapGenerator.h"
#include <algorithm>

MapGenerator::MapGenerator(int width, int height): mapWidth(width), mapHeight(height)
{
	level = nullptr;
}

MapGenerator::~MapGenerator()
{
	delete[] level;
}

char* MapGenerator::generateCaveMap(const int threshold, const int steps, const int underPop, const int overPop)
{
	initialiseCaveMap(threshold);

	for (int i = 0; i < steps; ++i) {
		simulationStep(underPop, overPop);
	}
	
	int walkableTileCount = std::count(level, level + (mapWidth * mapHeight), '.');

	if (static_cast<float>(walkableTileCount) / static_cast<float>(mapWidth * mapHeight) < 0.3) {
		generateCaveMap(threshold, steps, underPop, overPop);
	} else {
		connectCaves(floodFill());

		for (int j = 0; j < 3; ++j) {
			smoothMap(j);
		}

		smoothMap(3);
		fillBorder();
		hollowSolidChunks();

		
	}

	return level;
}

void MapGenerator::initialiseCaveMap(const int threshold)
{
	if (level == nullptr) {
		level = new char[mapWidth * mapHeight];
	}

	for (int i = 0; i < mapWidth * mapHeight; i++) {

		int chance = std::rand() % 100 + 1;
		if (chance >= threshold) {
			level[i] = '.';
		} else {
			level[i] = '#';
		}
	}
}

void MapGenerator::simulationStep(const int underPop, const int overPop)
{
	char* new_level = new char[mapWidth * mapHeight];

	for (int i = 0; i < mapWidth * mapHeight; i++) {
		int wallCount = getNeighbourWallCount(i);

		if (wallCount <= underPop || wallCount > overPop) {
			new_level[i] = '#';
		} else {
			new_level[i] = '.';
		}
	}

	delete[] level;
	level = new_level;
}

std::pair<int, int> MapGenerator::floodFill()
{
	int letterCode{ 97 };
	int j{ 0 };

	for (int i = 0; i < mapWidth * mapHeight; i++) {
		if (level[i] == '#') { 
			continue; 
		} else if (level[i] == '.') {
			std::vector<int> neighbours = getNeighbours(i);

			while (neighbours.size() > 0) {

				j = neighbours.back();
				neighbours.pop_back();

				if (level[j] == '.') {
					level[j] = static_cast<char>(letterCode);
					std::vector<int> newNeighbours = getNeighbours(j);
					for (auto k : newNeighbours) {
						if (std::find(neighbours.begin(), neighbours.end(), k) == neighbours.end() && level[k] == '.') {
							neighbours.push_back(k);
						}
					}
				}
			}
			letterCode++;
		}
	}

	return std::make_pair(97, letterCode);
}

void MapGenerator::connectCaves(std::pair<int, int> letterCodes)
{
	int startLetterCode = letterCodes.first;
	int endLetterCode = letterCodes.second;
	char mostFrequent{ 'a' };

	for (int i = startLetterCode; i < endLetterCode; ++i) {
		char letter = static_cast<char>(i);

		int letterCount = std::count(level, level + (mapWidth * mapHeight), letter);

		if (i == startLetterCode) {
			mostFrequent = letter;
		}else {
			if (std::count(level, level + (mapWidth * mapHeight), mostFrequent) < letterCount) {
				mostFrequent = letter;
			}
		}
	}

	int highestCount = std::count(level, level + (mapWidth * mapHeight), mostFrequent);

	for (int i = 0; i < mapWidth * mapHeight; ++i) {
		if (level[i] == mostFrequent) {
			level[i] = '.';
		} else {
			level[i] = '#';
		}
	}
}

void MapGenerator::smoothMap(const int minWallThreshold)
{
	int wallCount{ 0 };
	char* new_level = new char[mapWidth * mapHeight];

	for (int i = 0; i < mapWidth * mapHeight; i++) {
		if (i == 0 || i == mapWidth - 1 || i == mapWidth * mapHeight - 1 || i == mapWidth * mapHeight - mapWidth) {
			continue;
		}

		wallCount = getNeighbourWallCount(i);
		if (wallCount <= minWallThreshold) {
			new_level[i] = '.';
		} else {
			new_level[i] = level[i];
		}
	}

	delete[] level;
	level = new_level;
}

void MapGenerator::fillBorder()
{
	for (int i = 0; i < mapWidth * mapHeight; i++) {
		if (i % mapWidth == 0 || i % mapWidth == mapWidth - 1 || i < mapWidth || i > mapWidth * mapHeight - mapWidth) {
			level[i] = '#';
		}
	}
}

void MapGenerator::hollowSolidChunks()
{
	char* new_level = new char[mapWidth * mapHeight];

	for (int i = 0; i < mapWidth * mapHeight; i++) {
		int walkableCount{ 0 };

		if (level[i] == '.') {
			new_level[i] = level[i];
			continue;
		}

		std::vector<int> neighbours = getNeighbours(i);

		for (auto k : neighbours) {

			if (level[k] == '.') {
				walkableCount++;
			} 
		}

		if (walkableCount > 0) {
			new_level[i] = level[i];
		} else {
			new_level[i] = ' ';
		}
	}

	delete[] level;
	level = new_level;
}

std::vector<int> MapGenerator::getNeighbours(int i)
{
	int x = i % mapWidth;
	int y = i / mapWidth;
	std::vector<int> neighbours;

	for (int m = -1; m < 2; ++m) {
		for (int n = -1; n < 2; ++n) {
			if (n == 0 && m == 0) { continue; }

			if (x + m >= 0 && x + m < mapWidth && y + n >= 0 && y + n < mapHeight) {
				if (std::find(neighbours.begin(), neighbours.end(), i + m + n * mapWidth) == neighbours.end()) {

					neighbours.push_back((x + m) + (y + n) * mapWidth);
				}
			}
		}
	}

	return neighbours;
}

int MapGenerator::getNeighbourWallCount(int i)
{
	int wallCount{ 0 };

	int x = i % mapWidth;
	int y = i / mapWidth;

	for (int m = -1; m < 2; ++m) {
		for (int n = -1; n < 2; ++n) {
			if (n == 0 && m == 0) { continue; }

			if (x + m >= 0 && x + m < mapWidth && y + n >= 0 && y + n < mapHeight) {
				if (level[i + m + n * mapWidth] == '#') {
					++wallCount;
				}
			}
		}
	}

	return wallCount;
}


char* MapGenerator::generateRoomMap(const int minRooms, const int maxRooms, const int minRoomWidth, const int maxRoomWidth)
{
	return level;
}