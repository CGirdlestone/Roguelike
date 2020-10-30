#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>

namespace utils {
	void serialiseInt(std::ofstream& file, int x);
	void serialiseString(std::ofstream& file, std::string stringToSerialise);
	int deserialiseInt(char* buffer, int i);
	int advanceFourBytes(int i);
}


#endif
