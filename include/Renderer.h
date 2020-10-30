#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <map>
#include <string>
#include "Console.h"
#include "GameObject.h"
#include "Message.h"
#include "MessageLog.h"
#include "Camera.h"
#include "DungeonGenerator.h"
#include "Particle.h"

class Renderer
{
public:
	Renderer(Console* console);
	~Renderer();
	void drawActors(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors);
	void drawMap(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors);
	void drawLog(MessageLog* messageLog, int height);
	void drawUI();
	void drawMenuOutline();
	void drawGameScreen(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors, MessageLog* messageLog, std::vector<Particle> &particles);
	void drawPlayerInfo(GameObject* player, DungeonGenerator* dungeon);
	void drawStartMenu(int i, int options);
	void drawInventory(std::map<int, GameObject*> *actors, int i);
	void drawEquippedItem(std::string slot, std::string item, int y, int index);
	void drawCharacterScene(std::map<int, GameObject*> *actors, int index);
	void drawTargetingScene(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors, MessageLog* messageLog, int radius, std::vector<int> *path, int splashRadius, int _x, int _y);
	bool checkInRange(int x, int y, int i, int j, int radius){ return (x-i)*(x-i) + (y-j)*(y-j) <= radius*radius;};
	void drawMiniMap(DungeonGenerator* dungeon, std::map<int, GameObject*> *actors);
	void drawPauseMenu(int i, Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors, MessageLog* messageLog);
	void drawGameOver(int i, std::vector<std::string> &deathMessages, int totalLines);
	void drawParticles(Camera* camera, DungeonGenerator* dungeon, std::vector<Particle> &particles);

	void drawObject(GameObject* entity, int x, int y, int scale = 1);
	void drawTile(char* c, int x, int y, bool inView, int scale = 1);
	void drawTileMap(Camera* camera, DungeonGenerator* dungeon);
	void drawAsciiMap(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*>* actors);
	void autoTile(int x, int y, int i, int offsetI, int tile, Camera* camera, DungeonGenerator* dungeon);

	void drawBox(int x, int y, int width, int height);
	void drawText(std::string& text, int x, int y, bool highlighted);
	void drawText(std::string& text, int x, int y, SDL_Color colour);
	void drawBar(int x, int y, int width, int current, int max, SDL_Color colour);
	std::vector<std::string> wrapText(std::string& text, int width);
		
	void toggleAsciiMode();

	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
	
private:
	Console* m_console;
	SDL_Color m_defaultColour;
	SDL_Color m_inViewColour;
	SDL_Color m_borderColour;
	SDL_Color m_highlightColour;
	SDL_Color m_textColour;
};

#endif
