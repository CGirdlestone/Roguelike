#include "AnimationSystem.h"

AnimationSystem::AnimationSystem(EventManager* eventManager, std::map<int, GameObject*>* entities):
	m_eventManager(eventManager), m_entities(entities)
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

void AnimationSystem::update(Uint32 dt)
{
	for (Uint32 i = 0; i < m_entities->size(); ++i) {
		GameObject* entity = m_entities->at(i);
		if (entity->animation != nullptr) {
			updateEntity(dt, entity);
		}
	}
}