#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "ParticleSystem.h"
#include "Components.h"
#include "EventManager.h"
#include "Camera.h"

ParticleSystem::ParticleSystem(EventManager* eventManager, std::map<int, GameObject*> *entities):
m_eventManager(eventManager), m_entities(entities)
{
  m_eventManager->registerSystem(ONHIT, this);
  m_eventManager->registerSystem(USEITEM, this);
}

ParticleSystem::~ParticleSystem()
{
  m_eventManager = nullptr;
  m_entities = nullptr;
}


void ParticleSystem::addParticle(int _x, int _y, int _target_x, int _target_y, const std::string& effect)
{
	if (_target_x != -1 && _target_y != -1) {
		if (effect == "FIREBALL") {
			SDL_Color colour = { 0xda, 0x24, 0x24 };
			Renderable r = Renderable('*', colour, 0, 21, 29);
			Animation a = Animation();
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(0);
			a.sprite_x.push(0);
			a.sprite_y.push(21);
			a.sprite_y.push(21);
			Particle p = Particle((double)_x, (double)_y, _target_x, _target_y, 0.05, 0.05, r, a);
			particles.push_back(p);
		}
		else if (effect == "RANGED") {
			int sprite_x{ 0 };
			char c{ '|' };
			if (_target_x == _x) {

			}
			else if (_target_y == _y) {
				sprite_x = 1;
				c = '-';
			}
			else if ((_target_x > _x && _target_y > _y) || (_target_x > _x && _target_y < _y)) {
				sprite_x = 2;
				c = '\\';
			}
			else {
				sprite_x = 3;
				c = '/';
			}
			SDL_Color colour = { 0xde, 0x97, 0x51 };
			Renderable r = Renderable(c, colour, sprite_x, 20, 29);
			Animation a = Animation();
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(sprite_x);
			a.sprite_x.push(sprite_x);
			a.sprite_y.push(20);
			a.sprite_y.push(20);
			Particle p = Particle((double)_x, (double)_y, _target_x, _target_y, 0.05, 0.05, r, a);
			particles.push_back(p);
		}
	}
	else
	{
		if (effect == "HEALING") {
			SDL_Color colour = { 0xda, 0x24, 0x24 };
			Renderable r = Renderable('+', colour, 4, 21, 29);
			Animation a = Animation();
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(4);
			a.sprite_x.push(4);
			a.sprite_y.push(21);
			a.sprite_y.push(21);
			Particle p = Particle((double)_x, (double)_y, _x, _y-3, 0.05, 0.05, r, a);
			particles.push_back(p);
		}
	}
}

void ParticleSystem::update(Uint32 dt)
{
	if (particles.empty()) {
		return; 
	}

	for (Uint32 i = 0; i < particles.size(); ++i) {
		if (particles[i].target_x == -1) {
			ageParticle(dt, particles[i]);
		}
		moveParticle(dt, particles[i]);
	}

	particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle p) { return !p.is_alive; }), particles.end());
	
}

void ParticleSystem::moveParticle(Uint32 dt, Particle& particle)
{
	particle.x += particle.ax / 10;
	particle.y += particle.ay / 10;

	if ((int)particle.x >= particle.target_x - 1 && (int)particle.x <= particle.target_x + 1 &&
		(int)particle.y >= particle.target_y - 1 && (int)particle.y <= particle.target_y + 1) {
		particle.is_alive = false;
	}
}

void ParticleSystem::ageParticle(Uint32 dt, Particle& particle)
{
	particle.currentLife += dt;
	if (particle.currentLife > particle.lifetime) {
		particle.is_alive = false;
	}
}

// notify overrides below

void ParticleSystem::notify(UseItemEvent event)
{
	int x = m_entities->at(event.m_user_uid)->position->x;
	int y = m_entities->at(event.m_user_uid)->position->y;
	std::string effect = m_entities->at(event.m_item_uid)->useable->funcToDo;


	if (m_entities->at(event.m_item_uid)->useable->ranged) {
		if (event.m_target_uid != -1) {		
			int target_x = m_entities->at(event.m_target_uid)->position->x;
			int target_y = m_entities->at(event.m_target_uid)->position->y;

			addParticle(x, y, target_x, target_y, effect);
		}
	}
	else {
		addParticle(x, y, -1, -1, "HEALING");
	}
}

