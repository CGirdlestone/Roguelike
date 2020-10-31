#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>

namespace utils {
	void serialiseInt(std::ofstream& file, int x);
	void serialiseString(std::ofstream& file, std::string stringToSerialise);
	int deserialiseInt(char* buffer, int i);
	int deserialiseString(char* buffer, int i, std::string& stringToDeserialise);
	int advanceFourBytes(int i);
	int roll(int num_dice, int num_sides);
}


#endif
