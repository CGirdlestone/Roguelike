
#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include <map>
#include "System.h"
#include "Events.h"
#include "Components.h"
#include "GameObject.h"

class EventManager;

class PlayerSystem : public System
{
public:
	PlayerSystem(EventManager* eventManager, std::map<int, GameObject*> *entities);
	virtual ~PlayerSystem();
	void onExpGain(ExpGainEvent event);
	void checkLevelUp();
	void increaseThreshold();
	void increaseLevel();

	virtual void notify(ExpGainEvent event);
private:
	EventManager* m_eventManager;
	std::map<int, GameObject*> *m_entities;
};


#endif
