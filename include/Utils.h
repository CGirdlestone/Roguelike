#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <queue>

namespace utils {
	void serialiseInt(std::ofstream& file, int x);
	void serialiseString(std::ofstream& file, std::string stringToSerialise);
	void serialiseQueueInts(std::ofstream& file, std::queue<int>& queueToSerialise);
	int deserialiseInt(char* buffer, int i);
	int deserialiseString(char* buffer, int i, std::string& stringToDeserialise);
	int deserialiseQueueInts(char* buffer, int i, std::queue<int>& queueToDeserialise);
	int advanceFourBytes(int i);
	int roll(int num_dice, int num_sides);
	int roll(std::string& _roll);

	int getAttributeMod(int attrib);
}


#endif
