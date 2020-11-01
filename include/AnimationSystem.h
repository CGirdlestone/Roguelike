#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include <map>
#include "System.h"
#include "EventManager.h"
#include "Components.h"
#include "GameObject.h"

class EventManager;

class AnimationSystem : public System
{
public:
	AnimationSystem(EventManager* eventManager, std::map<int, GameObject*>* entities);
	virtual ~AnimationSystem();
	void update(Uint32 dt);
	void updateEntity(Uint32 dt, GameObject* entity);
	void shuffleFrame(GameObject* entity);

private:
	EventManager* m_eventManager;
	std::map<int, GameObject*>* m_entities;
};

#endif
