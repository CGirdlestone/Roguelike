#ifndef  SCRIPTS_H
#define SCRIPTS_H

#include "GameObject.h"
#include "Components.h"
#include "EventManager.h"
#include "DungeonGenerator.h"

namespace scripts
{
	namespace base
	{
		int heal(EventManager* event_manager, GameObject* entity, std::string& damage_roll );
		int damage(EventManager* event_manager, GameObject* entity, GameObject* target, std::string& damage_roll, DamageTypes type);
		bool set_status(EventManager* event_manager, GameObject* entity, GameObject* target, StatusTypes type, int damage, int duration);
		void damage_neighbours(EventManager* event_manager, int x, int y, int damage, DamageTypes type, DungeonGenerator* dungeon);
	}

	bool heal(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target = nullptr,  DungeonGenerator* dungeon = nullptr);
	bool fireball(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon = nullptr);
	bool lightning(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon = nullptr);
	bool lifeleech(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon = nullptr);
	bool poison(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon = nullptr);

	bool range_attack(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon);
}

#endif // ! SCRIPTS_H

