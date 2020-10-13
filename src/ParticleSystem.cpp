#include <cstdlib>
#include <map>
#include <iostream>
#include <algorithm>

#include "ParticleSystem.h"
#include "Components.h"
#include "EventManager.h"
#include "Events.h"
#include "EventTypes.h"

ParticleSystem::ParticleSystem(EventManager* eventManager, std::map<int, GameObject*> *entities):
m_eventManager(eventManager), m_entities(entities)
{
  m_eventManager->registerSystem(ONHIT, this);
  m_eventManager->registerSystem(ONMISS, this);
}

ParticleSystem::~ParticleSystem()
{
  m_eventManager = nullptr;
  m_entities = nullptr;
}


void ParticleSystem::addParticle(int _x, int _y, int _size, int _red, int _green, int _blue)
{
	Particle p = Particle(_x, _y, _size, _red, _green, _blue);
	particles.push_back(p);
}

void ParticleSystem::ageParticles(uint32_t dt)
{
	if (particles.empty()){ return; }


	/*std::vector<Particle>::iterator it = particles.begin();
	while (it != particles.end()){
	*/
	for (unsigned int i = 0; i < particles.size(); ++i){

		particles[i].currentLife -= dt;
		if (particles[i].currentLife <= 0){
			particles[i].steps += 1;
			particles[i].currentLife = particles[i].lifetime;
		}
	}

	particles.erase(std::remove_if(particles.begin(), particles.end()-1, [](Particle p) { return p.steps >= 6; }));
}

// notify overrides below

void ParticleSystem::notify(OnHitEvent event)
{
	int x = m_entities->at(event.m_defender_uid)->position->x; 
	int y = m_entities->at(event.m_defender_uid)->position->y;
	int size = 15;
	int red = 0x00;
	int green = 0x00;
	int blue = 0x00;
		
	addParticle(x, y, size, red, green, blue);
}

