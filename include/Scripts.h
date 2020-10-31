#ifndef  SCRIPTS_H
#define SCRIPTS_H

#include "GameObject.h"
#include "Components.h"
#include "EventManager.h"
#include "DamageTypes.h"
#include "DungeonGenerator.h"

namespace scripts
{
	namespace base
	{
		int heal(EventManager* event_manager, GameObject* entity, int num_dice, int num_sides);
		int damage(EventManager* event_manager, GameObject* entity, GameObject* target, int num_dice, int num_sides, DamageTypes type);
	}

	bool heal(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target = nullptr,  DungeonGenerator* dungeon = nullptr);
	bool fireball(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon = nullptr);
	bool lightning(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon = nullptr);

	bool range_attack(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon);
}

#endif // ! SCRIPTS_H

