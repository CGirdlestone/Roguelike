
#ifndef COMBATSYSTEM_H
#define COMBATSYSTEM_H

#include <map>
#include "System.h"
#include "Events.h"
#include "Components.h"
#include "GameObject.h"
#include "DamageTypes.h"

class EventManager;

class CombatSystem : public System
{
public:
	CombatSystem(EventManager* eventManager, std::map<int, GameObject*> *entities);
	virtual ~CombatSystem();
	void doAttack(AttackEvent event);
	void calculateDamage(OnHitEvent event);
	void calculateDamage(OnCriticalHitEvent event);
	void applyDamage(DamageEvent event);
	void onDead(DeadEvent event);
	bool isResistantToDamageType(GameObject* defender, DamageTypes type);
	bool isWeakToDamageType(GameObject* defender, DamageTypes type); 
	DamageTypes getDamageType(GameObject* attacker);
	void onTick();
	void update(Uint32 dt);

	virtual void notify(AttackEvent event);
	virtual void notify(OnHitEvent event);
	virtual void notify(OnCriticalHitEvent event);
	virtual void notify(DamageEvent event);
	virtual void notify(DeadEvent event);
private:
	EventManager* m_eventManager;
	std::map<int, GameObject*> *m_entities;
};


#endif
