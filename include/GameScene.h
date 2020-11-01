#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <map>
#include <fstream>
#include "SDL.h"
#include "EntityFactory.h"
#include "LootManager.h"
#include "Renderer.h"
#include "KeyPressSurfaces.h"
#include "EventManager.h"
#include "SceneTypes.h"
#include "Scene.h"
#include "GameObject.h"
#include "DungeonGenerator.h"
#include "Camera.h"
#include "MessageLog.h"
#include "ParticleSystem.h"
#include "CombatSystem.h"
#include "AnimationSystem.h"
#include "Slots.h"
#include "Pathfind.h"


class GameScene : public Scene
{
public:
	GameScene(EventManager *eventManager, Renderer *renderer, std::map<int, GameObject*> *entities, Camera* camera, DungeonGenerator* dungeon, MessageLog* messageLog, ParticleSystem* particleSystem, CombatSystem* combatSystem, AnimationSystem* animationSystem);
	virtual ~GameScene();
	enum KeyPressSurfaces getEvent(SDL_Event* e);
	void render();
	void handleInput(KeyPressSurfaces keyPress);
	void processEntities();
	void update(Uint32 dt);
	void onTick();
	void newGame();
	bool checkDescend();
	void nextLevel();
	void startOver();
	void finishPlayerTurn(){m_playerTurn = false; };
	void serialiseGameState(std::ofstream & file);
	int parseGameObjects(int i, char* buffer, std::fstream::pos_type length);
	int parseMap(int i, char* buffer, std::fstream::pos_type length);
	int parseExploredMap(int i, char* buffer, std::fstream::pos_type length);
	int parseDungeonDepth(int i, char* buffer, std::fstream::pos_type length);
	void mapUIDsToGameObjects();
	void saveGame();
	void loadGame();

private:
	EventManager *m_eventManager;
	Renderer *m_renderer;
	std::map<int, GameObject*> *m_entities;
	Camera* m_camera;
	DungeonGenerator *m_dungeon;
	MessageLog *m_messageLog;
	ParticleSystem *m_particleSystem;
	CombatSystem *m_combatSystem;
	AnimationSystem* m_animationSystem;
	bool m_playerTurn;
};

#endif
