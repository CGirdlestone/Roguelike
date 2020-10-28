#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <cstdint>
#include <vector>
#include <utility>

struct Rectangle
{
	Rectangle(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) { };
	int x, y, w, h;
};

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
	void fillBorder(char c);
	std::vector<int> getNeighbours(int i);
	int getNeighbourWallCount(int i);
	void hollowSolidChunks();

	// Room Maps - Position Rectangles
	void initialiseRoomMap();
	std::vector<Rectangle> generateAllRooms(const int minRooms, const int maxRooms, const int minRoomWidth, const int maxRoomWidth);
	Rectangle generateRoom(const int minRoomWidth, const int maxRoomWidth);
	bool placeRoom(Rectangle& rect, std::vector<Rectangle>& rooms);
	void writeRooms(std::vector<Rectangle>& rooms);
	void sortRooms(std::vector<Rectangle>& rooms);
	int getRoomExit(Rectangle& rect);
	void makeCorridor(int start, int finish);
	void tunnelHorizontally(int& start_x, const int start_y, const int finish_x);
	void tunnelVertically(const int start_x, int& start_y, const int finish_y);
	void connectRooms(std::vector<Rectangle>& rooms);
};

#endif
