#include <fstream>
#include <sstream>
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"

#include "Game.h"

using namespace std;

int main(int arc, char *argv[])
{
	std::ifstream file;
	std::stringstream ss;
	int mapWidth{ 0 }, mapHeight{ 0 }, screenWidth{ 0 }, screenHeight{ 0 }, tileSize{ 0 };
	std::string title;
	std::string line;

	file.open("./resources/config.txt");
	if (file.is_open()){
		getline(file, line);
		ss << line;	
		ss >> mapWidth >> mapHeight >> screenWidth >> screenHeight >> tileSize >> title;
	}
	else {
		exit(1);
	}

  Game* game = new Game();

  if(game->init(mapWidth, mapHeight, screenWidth, screenHeight, tileSize, title.c_str())){
      game->run();
  }
  return 0;
}
