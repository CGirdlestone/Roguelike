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

int ParticleSystem::getOrientation(int x, int y, int target_x, int target_y)
{
	int orientation{ 0 };
	if (target_x == x) {
		orientation = 0;
	}
	else if (target_y == y) {
		orientation = 1;
	}
	else if ((target_x > x && target_y > y) || (target_x < x && target_y < y)) {
		orientation = 2;
	}
	else {
		orientation = 3;
	}
	return orientation;
}


void ParticleSystem::addParticle(int _x, int _y, int _target_x, int _target_y, const std::string& effect)
{
	if (_target_x != -1 && _target_y != -1) {
		int sprite_x{ getOrientation(_x, _y, _target_x, _target_y) };
		if (effect == "FIREBALL") {
			char c{ '*' };

			SDL_Color colour = { 0xda, 0x24, 0x24 };
			Renderable r = Renderable(c, colour, sprite_x, 18, 29);
			Animation a = Animation();
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(sprite_x);
			a.sprite_x.push(sprite_x);
			a.sprite_y.push(18);
			a.sprite_y.push(18);
			Particle p = Particle((double)_x, (double)_y, _target_x, _target_y, 0.05, 0.05, r, a, effect);
			particles.push_back(p);
		}
		else if (effect == "RANGED") {
			char c[4] = { '|', '-', '\\', '/' };
			
			SDL_Color colour = { 0xde, 0x97, 0x51 };
			Renderable r = Renderable(c[sprite_x], colour, sprite_x, 20, 29);
			Animation a = Animation();
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(sprite_x);
			a.sprite_x.push(sprite_x);
			a.sprite_y.push(20);
			a.sprite_y.push(20);
			Particle p = Particle((double)_x, (double)_y, _target_x, _target_y, 0.05, 0.05, r, a, effect);
			particles.push_back(p);
		} 
	}
	else
	{
		if (effect == "HEALING") {
			SDL_Color colour = { 0xff, 0xff, 0xff };
			Renderable r = Renderable('+', colour, 6, 22, 29);
			Animation a = Animation();
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(6);
			a.sprite_x.push(6);
			a.sprite_y.push(22);
			a.sprite_y.push(22);
			for (int i = 0; i < 12; ++i) {
				int px{ _x + (std::rand() % 10) - 5 };
				int py{ _y + (std::rand() % 10) - 5 };
				Particle p = Particle((double)px, (double)py, _x, _y, 0.1, 0.1, r, a, effect);
				particles.push_back(p);
			}
		}
		else if (effect == "EXPLOSION") {
			char c{ '*' };
			int sprite_x = 1;
			SDL_Color colour = { 0xda, 0x24, 0x24 };
			Renderable r = Renderable(c, colour, sprite_x, 24, 29);
			Animation a = Animation();
			a.lifetime = 50;
			a.spriteSheets.push(29);
			a.spriteSheets.push(30);
			a.sprite_x.push(sprite_x);
			a.sprite_x.push(sprite_x);
			a.sprite_y.push(24);
			a.sprite_y.push(24);
			Particle p = Particle((double)_x, (double)_y, 150, 0, r, a, effect);
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
	particle.x += particle.ax * particle.vx;
	particle.y += particle.ay * particle.vy;

	if ((int)particle.x >= particle.target_x - 0 && (int)particle.x <= particle.target_x + 0 &&
		(int)particle.y >= particle.target_y - 0 && (int)particle.y <= particle.target_y + 0) {
		particle.is_alive = false;
		propogateParticleEffect(particle);
	}
}

void ParticleSystem::ageParticle(Uint32 dt, Particle& particle)
{
	particle.currentLife += dt;
	if (particle.currentLife > particle.lifetime) {
		particle.is_alive = false;
		propogateParticleEffect(particle);
	}
}

void ParticleSystem::propogateParticleEffect(Particle& particle)
{
	if (particle.effect == "FIREBALL") {
		std::string effect = "EXPLOSION";
		int x{ static_cast<int>(particle.x) };
		int y{ static_cast<int>(particle.y) };
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				addParticle(x + i - 1, y + j - 1, -1, -1, effect);
			}
		}
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
		addParticle(x, y, -1, -1, effect);
	}
}

