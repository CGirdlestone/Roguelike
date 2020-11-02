#include "AnimationSystem.h"

AnimationSystem::AnimationSystem(EventManager* eventManager, std::map<int, GameObject*>* entities, std::vector<Particle>& _particles):
	m_eventManager(eventManager), m_entities(entities), particles(_particles)
{

}

AnimationSystem::~AnimationSystem()
{

}

void AnimationSystem::shuffleFrame(GameObject* entity)
{
	int x{ 0 }, y{ 0 }, sheet{ 0 };

	x = std::move(entity->animation->sprite_x.front());
	entity->animation->sprite_x.pop();

	y = std::move(entity->animation->sprite_y.front());
	entity->animation->sprite_y.pop();

	sheet = std::move(entity->animation->spriteSheets.front());
	entity->animation->spriteSheets.pop();

	entity->renderable->spriteX = x;
	entity->renderable->spriteY = y;
	entity->renderable->sheet = sheet;

	entity->animation->sprite_x.push(std::move(x));
	entity->animation->sprite_y.push(std::move(y));
	entity->animation->spriteSheets.push(std::move(sheet));
}

void AnimationSystem::updateEntity(Uint32 dt, GameObject* entity)
{
	entity->animation->current_lifetime += dt;

	if (entity->animation->current_lifetime > entity->animation->lifetime) {
		entity->animation->current_lifetime = 0;
		shuffleFrame(entity);
	}
}

void AnimationSystem::shuffleFrame(Particle& particle)
{
	int x{ 0 }, y{ 0 }, sheet{ 0 };

	x = std::move(particle.animation.sprite_x.front());
	particle.animation.sprite_x.pop();

	y = std::move(particle.animation.sprite_y.front());
	particle.animation.sprite_y.pop();

	sheet = std::move(particle.animation.spriteSheets.front());
	particle.animation.spriteSheets.pop();

	particle.renderable.spriteX = x;
	particle.renderable.spriteY = y;
	particle.renderable.sheet = sheet;

	particle.animation.sprite_x.push(std::move(x));
	particle.animation.sprite_y.push(std::move(y));
	particle.animation.spriteSheets.push(std::move(sheet));
}

void AnimationSystem::updateParticle(Uint32 dt, Particle& particle)
{
	particle.animation.current_lifetime += dt;

	if (particle.animation.current_lifetime > particle.animation.lifetime) {
		particle.animation.current_lifetime = 0;
		shuffleFrame(particle);
	}
}

void AnimationSystem::update(Uint32 dt)
{
	std::map<int, GameObject*>::iterator iter;
	for (iter = m_entities->begin(); iter != m_entities->end(); ++iter) {
		GameObject* entity = iter->second;
		if (entity->animation != nullptr) {
			updateEntity(dt, entity);
		}
	}

	if (particles.size() > 0){
		for (Uint32 i = 0; i < particles.size(); i++)
		{
			updateParticle(dt, particles[i]);
		}
	}
}