
#ifndef INVENTORYSYSTEM_H
#define INVENTORYSYSTEM_H

#include <map>
#include <unordered_map>
#include <cstdlib>
#include <string>

#include "System.h"
#include "EventManager.h"
#include "Components.h"
#include "GameObject.h"
#include "UseableFunctionEnum.h"
#include "DungeonGenerator.h"
#include <functional>

class EventManager;

typedef bool (*script)(EventManager* event_manager, GameObject* item, GameObject* entity, GameObject* target, DungeonGenerator* dungeon);

class InventorySystem : public System
{
public:
	InventorySystem(EventManager* eventManager, std::map<int, GameObject*> *entities, DungeonGenerator *dungeon);
	virtual ~InventorySystem();
	void pickUpItem(TakeEvent event);
	void dropItem(DropEvent event);
	void equipItem(EquipEvent event);
	void unequipItem(UnequipEvent event);
	void useItem(UseItemEvent event);
	void decreaseUses(GameObject* item, int user_uid, int item_uid);

	void loadScripts();

	virtual void notify(DropEvent event);
	virtual void notify(TakeEvent event);
	virtual void notify(EquipEvent event);
	virtual void notify(UnequipEvent event);
	virtual void notify(UseItemEvent event);

private:
	EventManager* m_eventManager;
	std::map<int, GameObject*> *m_entities;
	DungeonGenerator *m_dungeon;
	std::unordered_map<std::string, script> m_scripts;
};


#endif
