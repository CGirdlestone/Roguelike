#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include <map>
#include "System.h"
#include "EventManager.h"
#include "Components.h"
#include "GameObject.h"
#include "Particle.h"

class EventManager;

class AnimationSystem : public System
{
public:
	AnimationSystem(EventManager* eventManager, std::map<int, GameObject*>* entities, std::vector<Particle>& particles);
	virtual ~AnimationSystem();
	void update(Uint32 dt);
	void updateEntity(Uint32 dt, GameObject* entity);
	void shuffleFrame(GameObject* entity);

	void updateParticle(Uint32 dt, Particle& particle);
	void shuffleFrame(Particle& particle);

private:
	EventManager* m_eventManager;
	std::map<int, GameObject*>* m_entities;
	std::vector<Particle> particles;
};

#endif
