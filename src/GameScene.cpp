#include <iostream>
#include "GameScene.h"
#include "Utils.h"


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
	m_dungeon->createMap(75, 5, 2, 6);
	m_dungeon->createPlayer(m_entities);
	m_dungeon->createMobs(m_entities);
	m_dungeon->createItems(m_entities);
	m_dungeon->placeStairs(m_entities);
	m_dungeon->populateGrid(m_entities);
	m_dungeon->RayCast(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
}

void GameScene::nextLevel()
{
	m_dungeon->descendDungeon(m_entities);
	m_dungeon->RayCast(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
	saveGame();
}

int GameScene::parseGameObjects(int i, char* buffer, std::fstream::pos_type length)
{
	while((long)i < length){
		GameObject* g = new GameObject();

		i = g->deserialise(buffer, i, length);
		m_entities->insert({g->m_uid, g});

		int count = 0;
		for (int j = 0; j < 4; ++j) {
			if (utils::deserialiseInt(buffer, i + j * 32) == 0) {
				count += 1;
			}
		}

		if (count == 4) {
			return i + 4 * 32;
		}
	}
	return i;
}

int GameScene::parseMap(int i, char* buffer, std::fstream::pos_type length)
{
	int num_tiles = m_dungeon->Getm_width() * m_dungeon->Getm_height();
	
	int letter;

	for (int j = 0; j < num_tiles; ++j){
		letter = utils::deserialiseInt(buffer, i);
		i = utils::advanceFourBytes(i);

		m_dungeon->m_level[j] = static_cast<char>(letter);
	}

	return i;
}

int GameScene::parseExploredMap(int i, char* buffer, std::fstream::pos_type length)
{
	int num_tiles = m_dungeon->Getm_width() * m_dungeon->Getm_height();
	
	int flag;

	for (int j = 0; j < num_tiles; ++j) {
		flag = utils::deserialiseInt(buffer, i);
		i = utils::advanceFourBytes(i);

		m_dungeon->m_exploredMap[j] = flag;
	}
	
	return i;
}

int GameScene::parseDungeonDepth(int i, char* buffer, std::fstream::pos_type length)
{
	m_dungeon->m_uid = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	return i;
}

void GameScene::serialiseGameState(std::ofstream& file)
{
	m_messageLog->serialise(file);

	utils::serialiseInt(file, m_dungeon->m_uid);

	for (int i = 0; i < m_dungeon->Getm_width() * m_dungeon->Getm_height(); ++i){
		utils::serialiseInt(file, static_cast<int>(m_dungeon->m_level[i]));
	}

	for (int i = 0; i < m_dungeon->Getm_width() * m_dungeon->Getm_height(); ++i){
		utils::serialiseInt(file, static_cast<int>(m_dungeon->m_exploredMap[i]));
	}

	std::map<int, GameObject*>::iterator it;
	for (it = m_entities->begin(); it != m_entities->end(); ++it) {
		it->second->serialise(file);
	}

	for (int i = 0; i < 4; ++i) {
		utils::serialiseInt(file, 0);
	}

}

void GameScene::saveGame()
{
	std::ofstream file("save.txt", std::ios::binary);

	serialiseGameState(file);

	file.close();
}

void GameScene::mapUIDsToGameObjects()
{
	int item_uid{ 0 };
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
	std::ifstream file("save.txt", std::ios::binary | std::ios::ate);

	file.seekg(0, std::ios::end);
	std::fstream::pos_type length = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = new char[length];

	file.read(buffer, length);

	file.close();
	
	int byteIndex = 0;

	m_dungeon->initialiseMap();

	byteIndex = m_messageLog->deserialise(buffer, byteIndex);
	byteIndex = parseDungeonDepth(byteIndex, buffer, length);
	byteIndex = parseMap(byteIndex, buffer, length);
	byteIndex = parseExploredMap(byteIndex, buffer, length);
	byteIndex = parseGameObjects(byteIndex, buffer, length);
	mapUIDsToGameObjects();
	m_dungeon->RayCast(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	m_dungeon->populateTileMap();
	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
}

void GameScene::processEntities()
{
	int j{ 0 };
	int x{ 0 }, y{ 0 };

	std::map<int, GameObject*>::iterator it;
  for (it = m_entities->begin(); it != m_entities->end(); ++it){
    if (it->second->ai != nullptr && it->second->fighter != nullptr){
      if (it->second->fighter->isAlive){
        if (m_dungeon->m_fovMap[it->second->position->x + it->second->position->y * m_dungeon->Getm_width()] == 1){
          it->second->ai->path.clear();
          aStar(m_dungeon->m_level, &it->second->ai->path, m_dungeon->Getm_width(), m_dungeon->Getm_height(), it->second->position->x, it->second->position->y, m_entities->at(0)->position->x, m_entities->at(0)->position->y);

		  if (it->second->ai->path.empty()) { continue; }

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

				case SDLK_KP_PLUS:
					return ZOOMIN;

				case SDLK_KP_MINUS:
					return ZOOMOUT;

				case SDLK_UP:
					return SCROLLUP;

				case SDLK_DOWN:
					return SCROLLDOWN;
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
	else if (keyPress == ZOOMIN) {
		int zoom = m_camera->getZoom();
		m_camera->setZoom(zoom + 1);
		m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
	}
	else if (keyPress == ZOOMOUT) {
		int zoom = m_camera->getZoom();
		m_camera->setZoom(zoom - 1);
		m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
	}
	else if (keyPress == SCROLLUP) {
		m_messageLog->scrollUp();
	}
	else if (keyPress == SCROLLDOWN) {
		m_messageLog->scrollDown();
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
	//m_messageLog->ageMessages(dt);
}

void GameScene::onTick()
{
	if (m_dungeon->recomputeFOV){
		m_dungeon->doRecomputeFOV(m_entities->at(0)->position->x, m_entities->at(0)->position->y, 10);
	}

	m_camera->updatePosition(m_entities->at(0)->position->x, m_entities->at(0)->position->y);
	
	m_combatSystem->onTick();
}
