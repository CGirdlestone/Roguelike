#include <cstdlib>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>
#include <iostream>

#include "SDL.h"

#include "EntityFactory.h"
#include "DungeonGenerator.h"
#include "Components.h"
#include "GameObject.h"
#include "DamageTypes.h"
#include "Slots.h"
#include "EntityType.h"
#include "Pathfind.h"

DungeonGenerator::DungeonGenerator(int width, int height, EntityFactory* factory):
m_width(width), m_height(height), m_factory(factory), m_mapGenerator(MapGenerator(width, height))
{
	m_uid = 0;
	recomputeFOV = false;
	m_level = nullptr;
	m_fovMap = nullptr;
	m_exploredMap = nullptr;

	m_factory->loadData("./resources/player.txt");
	m_factory->loadData("./resources/items.txt");
	m_factory->loadData("./resources/mobs.txt");
	m_factory->generateDistributions();

	for (int i = 0; i < width * height; ++i){
		std::vector<GameObject*> tileList;
		m_grid.insert({i, tileList});
	}

	std::srand(time(0));
}

DungeonGenerator::~DungeonGenerator()
{
	delete[] m_level;
	m_level = nullptr;

	delete[] m_fovMap;
	m_fovMap = nullptr;

	delete[] m_exploredMap;
	m_exploredMap = nullptr;

	delete m_factory;
	m_factory = nullptr;
}

void DungeonGenerator::initialiseMap()
{
	if(m_level == nullptr){
		m_level = new char[m_width * m_height];
	}

	if(m_fovMap == nullptr){
		m_fovMap = new int[m_width * m_height];
	}

	if(m_exploredMap == nullptr){
		m_exploredMap = new int[m_width * m_height];
	}


	for (int i = 0; i < m_width * m_height; i++){
		m_fovMap[i] = 0;
		m_exploredMap[i] = 0;
	}

	level_type = -1;
}

void DungeonGenerator::createMap(int threshold, int steps, int underPop, int overPop)
{
	initialiseMap();

	if (std::rand() % 100 + 1 < 5) {
		m_level = m_mapGenerator.generateCaveMap(threshold, steps, underPop, overPop);
		level_type = 0;
	} else {
		m_level = m_mapGenerator.generateRoomMap(8, 16, 5, 15);
		level_type = 1;
	}

}

float DungeonGenerator::getGradient(float x1, float y1, float x2, float y2)
{
	return (y1 - y2) / (x1 - x2);
}

int DungeonGenerator::transformX(int x, int y, int octant)
{
	if (octant == 2){
		return x;
	} else if (octant == 3){
		return -y;
	} else if (octant == 4){
		return -y;
	} else if (octant == 5){
		return -x;
	} else if (octant == 6){
		return -x;
	} else if (octant == 7){
		return y;
	} else if (octant == 8){
		return y;
	} else {
		return x;
	}
}

int DungeonGenerator::transformY(int x, int y, int octant)
{
	if (octant == 2){
		return -y;
	} else if (octant == 3){
		return x;
	} else if (octant == 4){
		return -x;
	} else if (octant == 5){
		return -y;
	} else if (octant == 6){
		return y;
	} else if (octant == 7){
		return -x;
	} else if (octant == 8){
		return x;
	} else {
		return y;
	}
}

bool DungeonGenerator::checkInMap(int x, int y)
{
	if(x >= 0 && x < m_width && y >= 0 && y < m_height){
		return true;
	} else {
		return false;
	}
}

void DungeonGenerator::castOctant(int x, int y, int radius, float bottomSlope, float topSlope, int step, int octant)
{
	int l, k; // offsets in the octant
	int a, b; // transformed offsets for octants 2 - 8
	float gradient, newBottomSlope;


	for(int i = 0; i < radius - step; i++){
		l = i + step;

		for (int j = 0; j < l + 1; j++){
			if (i==0 && j == 0){
				m_fovMap[y*m_width+x] = 1;
				m_exploredMap[y*m_width+x] = 1;
				continue;
			}

			k = j;

			a = transformX(l, k, octant);
			b = transformY(l, k, octant);

			if(!checkInMap(x+a, y+b)){
				continue;
			}

			gradient = getGradient(static_cast<float>(x+l), static_cast<float>(y+k), static_cast<float>(x), static_cast<float>(y));

			if (gradient < bottomSlope){
				continue;
			}

			if (gradient >= bottomSlope && gradient <= topSlope){
				if (m_level[(y+b)*m_width+x+a] != '.'){
					newBottomSlope = getGradient(static_cast<float>(x+l-0.5), static_cast<float>(y+k+0.5), static_cast<float>(x), static_cast<float>(y));
					castOctant(x, y, radius, newBottomSlope, topSlope, step, octant);

					topSlope = getGradient(static_cast<float>(x+l+0.5), static_cast<float>(y+k-0.5), static_cast<float>(x), static_cast<float>(y));
				}

				if (a*a + b*b <= radius*radius){
					m_fovMap[(y+b)*m_width+x+a] = 1;
					m_exploredMap[(y+b)*m_width+x+a] = 1;
				}
			}
		}
	}
}

void DungeonGenerator::shadowCast(int x, int y, int radius)
{
	for(int i = 1; i < 9; i++){
		castOctant(x, y, radius, 0, 1, 0, i);
	}
}

void DungeonGenerator::doRecomputeFOV(int x, int y, int radius)
{
	recomputeFOV = false;

	for (int i = 0; i < m_width * m_height; ++i){
		m_fovMap[i] = 0;
	}

	shadowCast(x, y, radius);
}

int DungeonGenerator::getFreePosition()
{
	int i{ 0 };
	
	while(true){
		i = std::rand()%(m_width * m_height);
		if (m_level[i] == '.'){
			break;
		}
	}
	return i;
}

void DungeonGenerator::createEntity(std::map<int, GameObject*> *actors, EntityType type)
{
	GameObject *entity = new GameObject();

	int i = getFreePosition();

	m_factory->makeEntity(m_uid, type, entity, i%m_width, i/m_width, actors);

	actors->insert({entity->m_uid, entity});
}

void DungeonGenerator::createPlayer(std::map<int, GameObject*> *actors)
{
	createEntity(actors, PLAYERENTITY);
	
	GameObject *entity = new GameObject();
	
	int i;
	int x, y;
	int healthPotionX, healthPotionY;

	while (true){
		i = actors->at(0)->position->x + actors->at(0)->position->y * m_width;
		x = rand() % 5 - 2;
		y = rand() % 5 - 2;
		
		healthPotionX = i % m_width + x;
		healthPotionY = i / m_width + y;
		if (m_level[healthPotionX + healthPotionY * m_width] == '.'){
			break;
		}
	}
	m_factory->makeEntity("Potion Of Healing", entity, healthPotionX, healthPotionY, actors);
	
	actors->insert({entity->m_uid, entity});
}

void DungeonGenerator::createMobs(std::map<int, GameObject*> *actors)
{
	int numMonstersMin = 8;
	int numMonstersMax = 16;

	int numMonsters = rand()%(numMonstersMax - numMonstersMin) + numMonstersMin;

	for (int i = 0; i < numMonsters; ++i){
		createEntity(actors, MOBENTITY);
	}
}

void DungeonGenerator::createItems(std::map<int, GameObject*> *actors)
{
	int numItemsMin = 8;
	int numItemsMax = 16;

	int numItems = rand()%(numItemsMax - numItemsMin) + numItemsMin;

	for (int i = 0; i < numItems; ++i){
		createEntity(actors, ITEMENTITY);
	}
}

void DungeonGenerator::placeStairs(std::map<int, GameObject*> *actors)
{
	GameObject *entity = new GameObject();	

	int i = getFreePosition();

	m_factory->makeStairs(entity, i%m_width, i/m_width);

	actors->insert({entity->m_uid, entity});
}

void DungeonGenerator::repositionPlayer(GameObject* player)
{
	int i = getFreePosition();
	player->position->x = i%m_width;
	player->position->y = i/m_width;	
}

void DungeonGenerator::descendDungeon(std::map<int, GameObject*> *actors)
{
	GameObject* player = actors->at(0);

	std::map<int, GameObject*> newActors;
	
	newActors.insert({0, player});

	for (int i = 0; i < static_cast<int>(player->inventory->inventory.size()); ++i){
		newActors.insert({player->inventory->inventory.at(i)->m_uid, player->inventory->inventory.at(i)});
	}

	std::map<EquipSlots, GameObject*>::iterator it;
	for (it = player->body->slots.begin(); it != player->body->slots.end(); ++it){
		if (it->second != nullptr){
			newActors.insert({it->second->m_uid, it->second});
		}
	}

	std::map<int, GameObject*>::iterator actorIt;
	for (actorIt = actors->begin(); actorIt != actors->end(); ){
		std::map<int, GameObject*>::iterator it = newActors.find(actorIt->first);
		if (it == newActors.end()){
			delete actorIt->second;
			actorIt->second = nullptr;
			actors->erase(actorIt++);
		} else {
			++actorIt;
		}
	}
	
	createMap(60, 6, 2, 5);

	clearGrid();

	repositionPlayer(player);

	createMobs(actors);
	createItems(actors);	
	placeStairs(actors);

	populateGrid(actors);
	++m_uid;
}

void DungeonGenerator::removeObjectFromTile(GameObject* entity, int i)
{
	std::vector<GameObject*>::iterator it = m_grid.at(i).begin();

	for (it = m_grid.at(i).begin(); it != m_grid.at(i).end(); ++it){
		if ((*it)->m_uid == entity->m_uid){
			break;
		}
	}
	if (it != m_grid.at(i).end()){
		m_grid.at(i).erase(it);
	}
}

void DungeonGenerator::moveObjectToTile(GameObject* entity, int i)
{
	if (m_grid.at(i).empty()){
		std::vector<GameObject*> vec;
		vec.push_back(entity);
		m_grid.at(i) = vec;
	} else {
		m_grid.at(i).push_back(entity);
	}
}

std::vector<GameObject*>& DungeonGenerator::getObjectsAtTile(int i)
{
	return m_grid.at(i);
}

void DungeonGenerator::clearGrid()
{
	std::map<int, std::vector<GameObject*>>::iterator it;

	for (it = m_grid.begin(); it != m_grid.end(); ++it){
		it->second.clear();
	}
}

void DungeonGenerator::populateGrid(std::map<int, GameObject*> *actors)
{
	std::map<int, GameObject*>::iterator it;

	for (it = actors->begin(); it != actors->end(); ++it){
		if (it->second->position != nullptr){
			m_grid.at(it->second->position->x + it->second->position->y * m_width).push_back(it->second);
		}
	}
}
