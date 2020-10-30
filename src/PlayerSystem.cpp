#include <cstdlib>
#include <map>

#include "PlayerSystem.h"
#include "EventManager.h"
#include "Events.h"
#include "EventTypes.h"

PlayerSystem::PlayerSystem(EventManager* eventManager, std::map<int, GameObject*> *entities):
m_eventManager(eventManager), m_entities(entities)
{
  m_eventManager->registerSystem(EXPGAIN, this);
}

PlayerSystem::~PlayerSystem()
{
  m_eventManager = nullptr;
  m_entities = nullptr;
}

void PlayerSystem::increaseThreshold()
{
	m_entities->at(0)->player->next += m_entities->at(0)->player->next * m_entities->at(0)->player->level;
}

void PlayerSystem::increaseLevel()
{
	m_entities->at(0)->player->level += 1;
}

void PlayerSystem::checkLevelUp()
{
	int expOverflow = m_entities->at(0)->player->exp - m_entities->at(0)->player->next;

	if (expOverflow >= 0){
		m_entities->at(0)->player->exp = expOverflow;
		increaseThreshold();
		increaseLevel();
	} 
}

void PlayerSystem::onExpGain(ExpGainEvent event)
{
	if (event.m_uid != 0){;

		m_entities->at(0)->player->exp += event.m_xp;

		checkLevelUp();
	}
}

// notify overrides below

void PlayerSystem::notify(ExpGainEvent event)
{
  onExpGain(event);
}
