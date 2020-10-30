
#include "MapGenerator.h"
#include <algorithm>
#include <cmath>

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
		fillBorder('#');
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

void MapGenerator::fillBorder(char c)
{
	for (int i = 0; i < mapWidth * mapHeight; i++) {
		if (i % mapWidth == 0 || i % mapWidth == mapWidth - 1 || i < mapWidth || i > mapWidth * mapHeight - mapWidth) {
			level[i] = c;
		}
	}
}

void MapGenerator::hollowSolidChunks()
{
	char* new_level = new char[mapWidth * mapHeight];

	for (int i = 0; i < mapWidth * mapHeight; i++) {
		int walkableCount{ 0 };

		if (getNeighbourWallCount(i) == 8) {
			new_level[i] = ' ';
		} else {
			new_level[i] = level[i];
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
	initialiseRoomMap();

	std::vector<Rectangle> rooms = generateAllRooms(minRooms, maxRooms, minRoomWidth, maxRoomWidth);

	writeRooms(rooms);

	sortRooms(rooms);

	connectRooms(rooms);

	return level;
}

void MapGenerator::initialiseRoomMap()
{
	if (level == nullptr) {
		level = new char[mapWidth * mapHeight];
	}

	for (int i = 0; i < mapWidth * mapHeight; ++i) {
		level[i] = '#';
	}
}

std::vector<Rectangle> MapGenerator::generateAllRooms(const int minRooms, const int maxRooms, const int minRoomWidth, const int maxRoomWidth)
{
	std::vector<Rectangle> rooms;

	int numRooms = std::rand() % (maxRooms - minRooms) + minRooms; // -1 because we've added the first room already

	while (numRooms > 0) {
		Rectangle room = generateRoom(minRoomWidth, maxRoomWidth);
		if (placeRoom(room, rooms)) {
			rooms.push_back(room);
			numRooms--;
		}
	}

	return rooms;
}

Rectangle MapGenerator::generateRoom(const int minRoomWidth, const int maxRoomWidth)
{
	int x{ std::rand() % mapWidth };
	int y{ std::rand() % mapHeight };
	int w{ std::rand() % (maxRoomWidth - minRoomWidth) + minRoomWidth };
	int h{ std::rand() % (maxRoomWidth - minRoomWidth) + minRoomWidth };

	return Rectangle(x, y, w, h);
}

bool MapGenerator::placeRoom(Rectangle& rect, std::vector<Rectangle>& rooms)
{
	int border{ 2 };
	// check the whole rect is in bounds
	if (!(rect.x + rect.w < mapWidth - border && rect.y + rect.h < mapHeight - border && rect.x > border && rect.y > border)) {
		return false;
	}

	// check for overlap
	for (const auto& room : rooms) {
		bool overlap{ (rect.x < room.x + room.w + border && rect.x + rect.w + border > room.x && rect.y < room.y + room.h + border && rect.y + rect.h + border > room.y) };

		if (overlap) {
			return false;
		}
	}

	return true;
}

void MapGenerator::writeRooms(std::vector<Rectangle>& rooms)
{
	for (const auto& room : rooms) {
		for (int w = 0; w < room.w; ++w) {
			for (int h = 0; h < room.h; ++h) {
				level[(room.x + w) + (room.y + h) * mapWidth] = '.';
			}
		}
	}
}


void MapGenerator::sortRooms(std::vector<Rectangle>& rooms)
{
	std::sort(rooms.begin(), rooms.end(), [](Rectangle& room1, Rectangle& room2) { return room1.x < room2.x; });
}

int MapGenerator::getRoomExit(Rectangle& rect)
{
	int x{ std::rand() % rect.w };
	int y{ std::rand() % rect.h };

	return rect.x + x + (rect.y + y) * mapWidth;
}

void MapGenerator::tunnelHorizontally(int& start_x, const int start_y, const int finish_x)
{
	do {
		level[start_x + start_y * mapWidth] = '.';

		if (start_x < finish_x) {
			start_x++;
		} else if (start_x > finish_x) {
			start_x--;
		}
	} while (!(start_x == finish_x));
}

void MapGenerator::tunnelVertically(const int start_x, int& start_y, const int finish_y)
{
	do {
		level[start_x + start_y * mapWidth] = '.';

		if (start_y < finish_y) {
			start_y++;
		}
		else if (start_y > finish_y) {
			start_y--;
		}
	} while (!(start_y == finish_y));
}

void MapGenerator::makeCorridor(int start, int finish)
{
	int start_x{ start % mapWidth };
	int start_y{ start / mapWidth }; 
	int finish_x{ finish % mapWidth };
	int finish_y{ finish / mapWidth };

	tunnelVertically(start_x, start_y, finish_y);
	tunnelHorizontally(start_x, start_y, finish_x);
}

void MapGenerator::connectRooms(std::vector<Rectangle>& rooms)
{
	while (rooms.size() > 1) {
		Rectangle exit_room = rooms.back();
		rooms.pop_back();
		Rectangle target_room = rooms.back();

		int exit_point = getRoomExit(exit_room);
		int target_point = getRoomExit(target_room);

		makeCorridor(exit_point, target_point);
	}
}