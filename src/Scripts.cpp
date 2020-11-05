
#include "Scripts.h"
#include "Utils.h"

int scripts::base::heal(EventManager* event_manager, GameObject* entity, std::string& damage_roll)
{
	int total = utils::roll(damage_roll);
	
	event_manager->pushEvent(DamageEvent(entity->m_uid, -1 * total));

	return total;
}


int scripts::base::damage(EventManager* event_manager, GameObject* entity, GameObject* target, std::string& damage_roll,  DamageTypes type)
{
	int damage = utils::roll(damage_roll);

	event_manager->pushEvent(DamageEvent(target->m_uid, damage));

	return damage;
}

bool scripts::base::set_status(EventManager* event_manager, GameObject* entity, GameObject* target, StatusTypes type, int damage, int duration)
{
	target->statusContainer->statuses[type].first = damage;
	target->statusContainer->statuses[type].second = duration;

	return true;
}

void scripts::base::damage_neighbours(EventManager* event_manager, int x, int y, int damage, DamageTypes type, DungeonGenerator* dungeon)
{
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (i == 0 && j == 0) { continue; }

			int n = (x + i) + (y + j) * dungeon->Getm_width();
			std::vector<GameObject*> entities = dungeon->getObjectsAtTile(n);

			if (entities.size() > 0) {
				for (GameObject* entity : entities) {
					if (entity->fighter != nullptr) {
						event_manager->pushEvent(DamageEvent(entity->m_uid, damage, type));
					}
				}
			}
		}
	}
}


bool scripts::heal(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	std::string roll = "2d8";
	if (entity->fighter->health < entity->fighter->maxHealth) {
		int total = base::heal(event_manager, entity, roll);
		return true;
	}

	return false;
}


bool scripts::fireball(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	std::string roll = "4d8";
	int total = base::damage(event_manager, entity, target, roll, FIRE);

	base::damage_neighbours(event_manager, target->position->x, target->position->y, total / 2, FIRE, dungeon);

	return true;
}


bool scripts::lightning(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	std::string roll = "2d12";
	int total = base::damage(event_manager, entity, target, roll, LIGHTNING);

	return true;
}


bool scripts::lifeleech(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon)
{
	std::string roll = "3d6";
	int total = base::damage(event_manager, entity, target, roll, NECROTIC);

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
	int total = base::damage(event_manager, entity, target, item->weapon->damage, item->weapon->damageType);

	return true;
}