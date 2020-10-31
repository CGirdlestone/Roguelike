
#include "Scripts.h"
#include "Utils.h"

int scripts::base::heal(EventManager* event_manager, GameObject* entity, int num_dice, int num_sides)
{
	int total = utils::roll(num_dice, num_sides);
	
	event_manager->pushEvent(DamageEvent(entity->m_uid, -1 * total));

	return total;
}

int scripts::base::damage(EventManager* event_manager, GameObject* entity,GameObject* target, int num_dice, int num_sides, DamageTypes type)
{
	int damage = utils::roll(num_dice, num_sides);

	event_manager->pushEvent(DamageEvent(target->m_uid, damage));

	return damage;
}

bool scripts::heal(EventManager* event_manager, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	if (entity->fighter->health < entity->fighter->maxHealth) {
		int total = base::heal(event_manager, entity, 2, 8);
		return true;
	}

	return false;

}

bool scripts::fireball(EventManager* event_manager, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	// get entities around the target and apply damage.

	int total = base::damage(event_manager, entity, target, 4, 8, FIRE);

	return true;
}

bool scripts::lightning(EventManager* event_manager, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	int total = base::damage(event_manager, entity, target, 2, 12, LIGHTNING);

	return true;
}

