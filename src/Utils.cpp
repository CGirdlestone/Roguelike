
#include "Utils.h"

void utils::serialiseInt(std::ofstream& file, int x)
{
	uint8_t bytes[4];
	bytes[0] = ((x >> 24) & 0xFF);
	bytes[1] = ((x >> 16) & 0xFF);
	bytes[2] = ((x >> 8) & 0xFF);
	bytes[3] = ((x >> 0) & 0xFF);

	for (int i = 0; i < 4; ++i) {
		file.write(reinterpret_cast<char*>(&bytes[i]), sizeof(reinterpret_cast<char*>(&bytes[i])));
	}
}

void utils::serialiseString(std::ofstream& file, std::string stringToSerialise)
{
	int size = static_cast<int>(stringToSerialise.length());
	int letterCode;

	serialiseInt(file, size);

	for (int i = 0; i < size; ++i) {
		letterCode = static_cast<int>(stringToSerialise[i]);
		serialiseInt(file, letterCode);
	}
}

int utils::deserialiseInt(char* buffer, int i)
{
	int value = ((unsigned char)buffer[i + 0] << 24 |
		(unsigned char)buffer[i + 1] << 16 |
		(unsigned char)buffer[i + 2] << 8 |
		(unsigned char)buffer[i + 3]
		);

	return value;
}

int utils::deserialiseString(char* buffer, int i, std::string& stringToDeserialise)
{
	int _strLength = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	/* Read in the entity's name */
	char* str_arr = new char[_strLength];
	for (int k = 0; k < _strLength; ++k) {
		int letterCode = utils::deserialiseInt(buffer, i);
		i = utils::advanceFourBytes(i);
		str_arr[k] = static_cast<char>(letterCode);
	}
	stringToDeserialise.assign(str_arr, _strLength);
	delete[] str_arr;

	return i;
}

int utils::advanceFourBytes(int i)
{
	return i += 32;
}