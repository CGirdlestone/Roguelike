
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <map>
#include <vector>
#include "System.h"
#include "Events.h"
#include "Particle.h"
#include "Camera.h"

class EventManager;

class ParticleSystem : public System
{
public:
	ParticleSystem(EventManager* eventManager, std::map<int, GameObject*> *entities);
	virtual ~ParticleSystem();
	void addParticle(int _x, int _y, int _target_x, int _target_y, const std::string& effect);
	void ageParticle(Uint32 dt, Particle& particle);
	void moveParticle(Uint32 dt, Particle& particle);
	void update(Uint32 dt);
	void purgeParticles() { particles.clear(); };
	int getOrientation(int x, int y, int target_x, int target_y);
	void propogateParticleEffect(Particle& particle);

  virtual void notify(UseItemEvent event);

	std::vector<Particle> particles;
private:
	EventManager* m_eventManager;
	std::map<int, GameObject*> *m_entities;
};


#endif
