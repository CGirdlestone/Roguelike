
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

bool scripts::base::set_status(EventManager* event_manager, GameObject* entity, GameObject* target, StatusTypes type, int damage, int duration)
{
	target->statusContainer->statuses[type].first = damage;
	target->statusContainer->statuses[type].second = duration;

	return true;
}


bool scripts::heal(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	if (entity->fighter->health < entity->fighter->maxHealth) {
		int total = base::heal(event_manager, entity, 2, 8);
		return true;
	}

	return false;
}


bool scripts::fireball(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	// TO DO get entities around the target and apply damage.

	int total = base::damage(event_manager, entity, target, 4, 8, FIRE);

	return true;
}


bool scripts::lightning(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	int total = base::damage(event_manager, entity, target, 2, 12, LIGHTNING);

	return true;
}


bool scripts::lifeleech(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	int total = base::damage(event_manager, entity, target, 2, 4, NECROTIC);

	int healing = total / 2;

	if (healing > 0) {
		if (entity->fighter->health < entity->fighter->maxHealth) {
			event_manager->pushEvent(DamageEvent(entity->m_uid, -1 * healing));
		}
	}

	return true;
}

bool scripts::poison(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	if (utils::roll(1, 100) < 10) {
		base::set_status(event_manager, entity, target, POISONED, 4, utils::roll(3,3));
		return true;
	}
	return false;
}


bool scripts::range_attack(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	int total = base::damage(event_manager, entity, target, 1, item->weapon->sidedDie, item->weapon->damageType);

	return true;
}