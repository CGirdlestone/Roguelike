
#include <map>
#include <iostream>
#include "StartScene.h"
#include "SDL.h"
#include "Renderer.h"
#include "KeyPressSurfaces.h"
#include "Pathfind.h"
#include "EventManager.h"
#include "Events.h"
#include "EventTypes.h"
#include "SceneTypes.h"
#include "TargetingScene.h"
#include "Pathfind.h"

TargetingScene::TargetingScene(EventManager *eventManager, Renderer *renderer, std::map<int, GameObject*> *entities, Camera *camera, DungeonGenerator *dungeon, MessageLog* messageLog):
m_eventManager(eventManager), m_renderer(renderer), m_entities(entities), m_camera(camera), m_dungeon(dungeon), m_messageLog(messageLog), m_item_uid(0), m_user_uid(0), m_x(0), m_y(0)
{

}

TargetingScene::~TargetingScene()
{

}

enum KeyPressSurfaces TargetingScene::getEvent(SDL_Event *e)
{
	while(SDL_PollEvent(e)){
  		if (e->type == SDL_QUIT){
    		return ESCAPE;
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
		  
				case SDLK_RETURN:
				return USE;
			}
  		}
	}
	return NONE;
}

void TargetingScene::handleInput(KeyPressSurfaces keyPress)
{
	int targetUID;
  
	if (keyPress == ESCAPE){
		m_eventManager->pushEvent(PopScene(1));
		bresenhamLine.clear();
	}else if (keyPress == NORTH){
		m_y -= 1;
		onTick();
	} else if (keyPress == SOUTH){
		m_y += 1;
		onTick();
	} else if (keyPress == WEST){
		m_x -= 1;
		onTick();
	} else if (keyPress == EAST){
		m_x += 1;
		onTick();
	} else if (keyPress == SOUTHEAST){
		m_y += 1;
		m_x += 1;
		onTick();
	} else if (keyPress == SOUTHWEST){
		m_y += 1;
		m_x -= 1;
		onTick();
	} else if (keyPress == NORTHWEST){
		m_y -= 1;
		m_x -= 1;
		onTick();
	} else if (keyPress == NORTHEAST){
		m_y -= 1;
		m_x +=1 ;
		onTick();
	} else if (keyPress == USE){
		targetUID = getTargetUID();
		if (targetUID != -1){
			if (m_entities->at(targetUID)->fighter != nullptr){
				if (m_entities->at(targetUID)->fighter->isAlive){
					m_eventManager->pushEvent(UseItemEvent(m_user_uid, m_item_uid, targetUID));
					m_eventManager->pushEvent(PopScene(1));
					bresenhamLine.clear();
				}
			}
		}
	}
}

int TargetingScene::getTargetUID()
{
	int radius{ 10 };
	
	std::vector<GameObject*> entities = m_dungeon->getObjectsAtTile(m_x + m_y * m_dungeon->Getm_width());

	std::vector<GameObject*>::iterator it;

	for (it = entities.begin(); it != entities.end(); ++it) {
		if ((*it)->fighter != nullptr) {
			if (checkInRange(m_x, m_y, m_entities->at(m_user_uid)->position->x, m_entities->at(m_user_uid)->position->y, radius)) {
				return (*it)->m_uid;
			}
		}
	}
	return -1;
}

void TargetingScene::render()
{
	int radius{ 10 };
	int splashRadius{ 0 };
	if (m_entities->at(m_item_uid)->useable->AOE)
	{
		splashRadius = 1;
	}
	
	m_renderer->drawTargetingScene(m_camera, m_dungeon, m_entities, m_messageLog, radius, &bresenhamLine, splashRadius, m_x, m_y);
}

void TargetingScene::update(Uint32 dt)
{

}

void TargetingScene::onTick()
{
	bresenhamLine.clear();
	getBresenhamLine(&bresenhamLine, m_dungeon->Getm_width(), m_x, m_y, m_entities->at(0)->position->x, m_entities->at(0)->position->y);	
}
