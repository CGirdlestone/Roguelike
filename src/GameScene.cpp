#include <iostream>
#include "GameScene.h"


GameScene::GameScene(EventManager *eventManager, Renderer *renderer, std::map<int, GameObject*> *entities, Camera* camera, DungeonGenerator* dungeon, MessageLog* messageLog, ParticleSystem* particleSystem, CombatSystem* combatSystem):
m_eventManager(eventManager), m_renderer(renderer), m_entities(entities), m_camera(camera), m_dungeon(dungeon), m_messageLog(messageLog), m_particleSystem(particleSystem), m_combatSystem(combatSystem)
{
  m_playerTurn = true;
}

GameScene::~GameScene()
{

}

void GameScene::startOver()
{
	std::map<int, GameObject*>::iterator it;
	for (it = m_entities->begin(); it != m_entities->end(); ){
		delete it->second;
		m_entities->erase(it++);
	}
	m_dungeon->clearGrid();
	m_dungeon->reset();
	m_messageLog->purgeLog();
	m_particleSystem->purgeParticles();
}

void GameScene::newGame()
{
	m_dungeon->createMap(60, 6, 2, 5);
	m_dungeon->createPlayer(m_entities);
	m_dungeon->createMobs(m_entities);
	m_dungeon->createItems(m_entities);
	m_dungeon->placeStairs(m_entities);
	m_dungeon->populateGrid(m_entities);
	m_dungeon->shadowCast(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
}

void GameScene::nextLevel()
{
	m_dungeon->descendDungeon(m_entities);
	m_dungeon->shadowCast(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
	saveGame();
}

int GameScene::parseGameObjects(int i, char* buffer, int length)
{
	while(i < length){
		GameObject* g = new GameObject();

		i = g->deserialise(buffer, i, length);
		m_entities->insert({g->m_uid, g});

		int count = 0;
		for (int j = 0; j < 4; ++j) {
			if (deserialiseInt(buffer, i + j * 32) == 0) {
				count += 1;
			}
			std::cout << deserialiseInt(buffer, i + j * 32) << std::endl;
			std::cout << count << std::endl;
		}

		if (count == 4) {
			return i + 4 * 32;
		}
		std::cout << std::endl;
	}
	return i;
}

int GameScene::parseMap(int i, char* buffer, int length)
{
	int numBytes = 4;
	int num_tiles = m_dungeon->Getm_width() * m_dungeon->Getm_height();
	char* level = new char[num_tiles];
	
	int letter;

	for (int j = 0; j < num_tiles; ++j){
		letter = deserialiseInt(buffer, i);
		i = advanceFourBytes(i);

		level[j] = static_cast<char>(letter);
	}
	
	for (int l = 0; l < m_dungeon->Getm_width() * m_dungeon->Getm_height(); ++l){
		m_dungeon->m_level[l] = level[l];
	}

	delete[] level;

	return i;
}

int GameScene::parseExploredMap(int i, char* buffer, int length)
{
	int numBytes = 4;
	int num_tiles = m_dungeon->Getm_width() * m_dungeon->Getm_height();
	char* exploredLevel = new char[num_tiles];
	
	int flag;

	for (int j = 0; j < num_tiles; ++j) {
		flag = deserialiseInt(buffer, i);
		i = advanceFourBytes(i);

		exploredLevel[j] = flag;
	}
	
	for (int j = 0; j < m_dungeon->Getm_width() * m_dungeon->Getm_height(); ++j){
		m_dungeon->m_exploredMap[j] = exploredLevel[j];
	}

	delete[] exploredLevel;

	return i;
}

int GameScene::parseDungeonDepth(int i, char* buffer, int length)
{
	int numBytes = 4;

	m_dungeon->m_uid = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}

void GameScene::serialiseGameState(std::vector<uint8_t> &byteVector)
{
	serialiseInt(byteVector, m_dungeon->m_uid);

	for (int i = 0; i < m_dungeon->Getm_width() * m_dungeon->Getm_height(); ++i){
		serialiseInt(byteVector, static_cast<int>(m_dungeon->m_level[i]));
	}

	for (int i = 0; i < m_dungeon->Getm_width() * m_dungeon->Getm_height(); ++i){
		serialiseInt(byteVector, static_cast<int>(m_dungeon->m_exploredMap[i]));
	}

	std::map<int, GameObject*>::iterator it;

	for (it = m_entities->begin(); it != m_entities->end(); ++it) {
		it->second->serialise(byteVector);
	}

	for (int i = 0; i < 4; ++i) {
		serialiseInt(byteVector, 0);
	}

}

void GameScene::saveGame()
{
	std::vector<uint8_t> byteVector;

	serialiseGameState(byteVector);

	std::ofstream file("save.txt");

	for (int i = 0; i < static_cast<int>(byteVector.size()); ++i){
		file.write(reinterpret_cast<char*>(&byteVector.at(i)), sizeof(reinterpret_cast<char*>(&byteVector.at(i))));
	}

	file.close();
}

void GameScene::mapUIDsToGameObjects()
{
	int item_uid;
	for (int i = 0; i < static_cast<int>(m_entities->at(0)->inventory->inventoryMirror.size()); ++i){
		item_uid = m_entities->at(0)->inventory->inventoryMirror.at(i);
		m_entities->at(0)->inventory->inventory.push_back(m_entities->at(item_uid));
	}

	std::map<int, int>::iterator it;
	for (it = m_entities->at(0)->body->slotsMirror.begin(); it != m_entities->at(0)->body->slotsMirror.end(); ++it){
		if (it->second == 0) { continue; }

		m_entities->at(0)->body->slots[static_cast<EquipSlots>(it->first)] = m_entities->at(it->second);
	}
}

void GameScene::loadGame()
{
	std::ifstream file("save.txt");

	file.seekg(0, file.end);
	int length = file.tellg();
	file.seekg(0, file.beg);

	char* buffer = new char[length];

	file.read(buffer, length);

	file.close();
	
	int byteIndex = 0;

	m_dungeon->initialiseMap(60);
	byteIndex = parseDungeonDepth(byteIndex, buffer, length);
	byteIndex = parseMap(byteIndex, buffer, length);
	byteIndex = parseExploredMap(byteIndex, buffer, length);
	byteIndex = parseGameObjects(byteIndex, buffer, length);
	mapUIDsToGameObjects();
	m_dungeon->shadowCast(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
}

void GameScene::processEntities()
{
  int j;
  int x, y;

	std::map<int, GameObject*>::iterator it;
  for (it = m_entities->begin(); it != m_entities->end(); ++it){
    if (it->second->ai != nullptr && it->second->fighter != nullptr){
      if (it->second->fighter->isAlive){
        if (m_dungeon->m_fovMap[it->second->position->x + it->second->position->y * m_dungeon->Getm_width()] == 1){
          it->second->ai->path.clear();
          aStar(m_dungeon->m_level, &it->second->ai->path, m_dungeon->Getm_width(), m_dungeon->Getm_height(), it->second->position->x, it->second->position->y, m_entities->at(0)->position->x, m_entities->at(0)->position->y);

          j = it->second->ai->path.back();
          it->second->ai->path.pop_back();

          x = j % m_dungeon->Getm_width();
          y = j / m_dungeon->Getm_width();

          MoveEvent moveEvent = MoveEvent(x - it->second->position->x, y - it->second->position->y, it->first);
          m_eventManager->pushEvent(moveEvent);

        } else {
          if (it->second->ai->path.size() > 0){
            j = it->second->ai->path.back();
            it->second->ai->path.pop_back();

            x = j % m_dungeon->Getm_width();
            y = j / m_dungeon->Getm_width();

            MoveEvent moveEvent = MoveEvent(x - it->second->position->x, y - it->second->position->y, it->first);
            m_eventManager->pushEvent(moveEvent);
          }
        }
      }
    }
  }
	m_playerTurn = true;
	onTick();
}

bool GameScene::checkDescend()
{
	std::map<int, GameObject*>::iterator iter;

	for (iter = m_entities->begin(); iter != m_entities->end(); ++iter){
		if (iter->second->stairs != nullptr){
			if (iter->second->position->x == m_entities->at(0)->position->x && iter->second->position->y == m_entities->at(0)->position->y){
				return true;
			}
		} 
	}
	return false;
}

enum KeyPressSurfaces GameScene::getEvent(SDL_Event *e)
{
  while(SDL_PollEvent(e)){
      if (e->type == SDL_QUIT){
          return EXITGAME;
      } else if (e->type == SDL_KEYDOWN){
          switch(e->key.keysym.sym){
              case SDLK_UP:
              return NORTH;

              case SDLK_DOWN:
              return SOUTH;

              case SDLK_LEFT:
              return WEST;

              case SDLK_RIGHT:
              return EAST;

              case SDLK_KP_1:
              return SOUTHWEST;

              case SDLK_KP_2:
              return SOUTH;

              case SDLK_KP_3:
              return SOUTHEAST;

              case SDLK_KP_4:
              return WEST;

              case SDLK_KP_5:
              return WAIT;

              case SDLK_KP_6:
              return EAST;

              case SDLK_KP_7:
              return NORTHWEST;

              case SDLK_KP_8:
              return NORTH;

              case SDLK_KP_9:
              return NORTHEAST;

              case SDLK_ESCAPE:
              return ESCAPE;

              case SDLK_F1:
              return F1;

              case SDLK_g:
              return GRAB;

							case SDLK_i:
							return BAG;

							case SDLK_c:
							return SHOWCHARSCREEN;

							case SDLK_RETURN:
							return USE;
							
							case SDLK_s:
							return SHOOT;

							case SDLK_F11:
							return ASCII;
          }
      }
  }
  return NONE;
}

void GameScene::handleInput(KeyPressSurfaces keyPress)
{
  if (keyPress == ESCAPE){
      m_eventManager->pushEvent(PushScene(PAUSEMENU));
  } else if (keyPress == WEST){
    MoveEvent moveEvent = MoveEvent(-1, 0, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == EAST){
    MoveEvent moveEvent = MoveEvent(1, 0, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == NORTH){
    MoveEvent moveEvent = MoveEvent(0, -1, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == SOUTH){
    MoveEvent moveEvent = MoveEvent(0, 1, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == NORTHWEST){
    MoveEvent moveEvent = MoveEvent(-1, -1, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == NORTHEAST){
    MoveEvent moveEvent = MoveEvent(1, -1, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == SOUTHWEST){
    MoveEvent moveEvent = MoveEvent(-1, 1, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == SOUTHEAST){
    MoveEvent moveEvent = MoveEvent(1, 1, 0);
    m_eventManager->pushEvent(moveEvent);
    m_playerTurn = false;
  } else if (keyPress == WAIT){
    m_playerTurn = false;
  } else if (keyPress == GRAB){
    TakeEvent takeEvent = TakeEvent(0, m_entities->at(0)->position->x, m_entities->at(0)->position->y);
    m_eventManager->pushEvent(takeEvent);
    m_playerTurn = false;
  } else if (keyPress == BAG){
		m_eventManager->pushEvent(PushScene(INVENTORY));
	} else if (keyPress == SHOWCHARSCREEN){
		m_eventManager->pushEvent(PushScene(CHARACTER));
	} else if (keyPress == USE){
		if(checkDescend()){
			nextLevel();
		}
	} else if (keyPress == ASCII){ 
		m_renderer->toggleAsciiMode();
	} else if (keyPress == EXITGAME){
		m_eventManager->pushEvent(QuitEvent());
	} else if (keyPress == SHOOT){
		if (m_entities->at(0)->body->slots.at(LEFTHAND) != nullptr){
			if (m_entities->at(0)->body->slots.at(LEFTHAND)->damage != nullptr && m_entities->at(0)->body->slots.at(LEFTHAND)->useable != nullptr){
				m_eventManager->pushEvent(UseItemEvent(0, m_entities->at(0)->body->slots.at(LEFTHAND)->m_uid));
			} 
		}
	}

  if (!m_playerTurn){
    processEntities();
  }
}


void GameScene::render()
{
  m_renderer->drawGameScreen(m_camera, m_dungeon, m_entities, m_messageLog, m_particleSystem->particles);
}

void GameScene::update(Uint32 dt)
{
	m_particleSystem->ageParticles(dt);
  m_messageLog->ageMessages(dt);
}

void GameScene::onTick()
{
  if (m_dungeon->recomputeFOV){
    m_dungeon->doRecomputeFOV(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
 	}

  m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
	
	m_combatSystem->onTick();
}
