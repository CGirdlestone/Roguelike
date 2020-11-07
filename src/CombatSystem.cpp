#include <iostream>
#include <cstdlib>
#include <map>

#include "CombatSystem.h"
#include "EventManager.h"
#include "Events.h"
#include "EventTypes.h"
#include "DamageTypes.h"
#include "Slots.h"
#include "Utils.h"

CombatSystem::CombatSystem(EventManager* eventManager, std::map<int, GameObject*> *entities):
m_eventManager(eventManager), m_entities(entities)
{
  m_eventManager->registerSystem(ATTACK, this);
  m_eventManager->registerSystem(ONHIT, this);
  m_eventManager->registerSystem(DAMAGE, this);
  m_eventManager->registerSystem(DEAD, this);
  m_eventManager->registerSystem(SETSTATUS, this);
}

CombatSystem::~CombatSystem()
{
  m_eventManager = nullptr;
  m_entities = nullptr;
}

void CombatSystem::doAttack(AttackEvent event)
{
	GameObject* attacker = m_entities->at(event.m_attacker_uid);
	GameObject* defender = m_entities->at(event.m_defender_uid);

	if (utils::roll(1, 20) + utils::getAttributeMod(attacker->fighter->strength) >= defender->fighter->armour_class ){
		OnHitEvent onHit = OnHitEvent(event.m_attacker_uid, event.m_defender_uid);
		m_eventManager->pushEvent(onHit);
	

		if (defender->fighter != nullptr) {
			if (defender->fighter->isAlive) {
				// check if the attacking entity has a melee weapon with a useable component
				// if it does, send a use item event.
				if (attacker->body != nullptr) {
					if (m_entities->at(event.m_attacker_uid)->body->slots[RIGHTHAND] != nullptr) {
						GameObject* item = attacker->body->slots[RIGHTHAND];
						if (item->useable != nullptr) {
							if (item->useable->ranged) {
								m_eventManager->pushEvent(UseItemEvent(event.m_attacker_uid, item->m_uid, event.m_defender_uid));
							}
							else {
								m_eventManager->pushEvent(UseItemEvent(event.m_attacker_uid, item->m_uid, -1));
							}
						}
					}
				}
			}
			
		}
	} else {
		OnMissEvent onMiss = OnMissEvent(event.m_attacker_uid, event.m_defender_uid);
		m_eventManager->pushEvent(onMiss);
	}
}

void CombatSystem::calculateDamage(OnHitEvent event)
{
	GameObject* attacker = m_entities->at(event.m_attacker_uid);
	GameObject* defender = m_entities->at(event.m_defender_uid);
	int dmg{ utils::getAttributeMod(attacker->fighter->strength) };
	
	if (attacker->body != nullptr) {
		dmg += attacker->body->slots[RIGHTHAND] != nullptr ? utils::roll(attacker->body->slots[RIGHTHAND]->weapon->damage) : utils::roll(1, 4);
	}
	else {
		if (attacker->ai != nullptr) {
			dmg += utils::roll(attacker->ai->damage);
		}
	}

	DamageTypes type = getDamageType(attacker);

	DamageEvent damageEvent = DamageEvent(event.m_defender_uid, (dmg < 1 ? 1 : dmg), type);
	m_eventManager->pushEvent(damageEvent);
}

void CombatSystem::calculateDamage(OnCriticalHitEvent event)
{
	GameObject* attacker = m_entities->at(event.m_attacker_uid);
	GameObject* defender = m_entities->at(event.m_defender_uid);
	int dmg{ utils::getAttributeMod(attacker->fighter->strength) };

	if (attacker->body != nullptr) {
		for (int i = 0; i < 2; ++i) {
			dmg += attacker->body->slots[RIGHTHAND] != nullptr ? utils::roll(attacker->body->slots[RIGHTHAND]->weapon->damage) : utils::roll(1, 4);

		}
	}
	else {
		if (attacker->ai != nullptr) {
			for (int i = 0; i < 2; ++i) {
				dmg += utils::roll(attacker->ai->damage);
			}
		}
	}

	DamageTypes type = getDamageType(attacker);

	DamageEvent damageEvent = DamageEvent(event.m_defender_uid, (dmg < 1 ? 1 : dmg), type);
	m_eventManager->pushEvent(damageEvent);
}

void CombatSystem::applyDamage(DamageEvent event)
{
	GameObject* entity = m_entities->at(event.m_uid);
	int dmg{ event.m_damage };
	if (entity->fighter != nullptr){

		if (isResistantToDamageType(entity, event.m_dmg_type)) {
			dmg = dmg / 2;
		}
		if (isWeakToDamageType(entity, event.m_dmg_type)) {
			dmg = dmg * 2;
		}

		entity->fighter->health -= event.m_damage;

		if (entity->fighter->health <= 0){
			entity->fighter->isAlive = false;
			DeadEvent deadEvent = DeadEvent(event.m_uid);
			m_eventManager->pushEvent(deadEvent);
		} else if (entity->fighter->health > entity->fighter->maxHealth){
			entity->fighter->health = entity->fighter->maxHealth;
		}
	}
}

DamageTypes CombatSystem::getDamageType(GameObject* attacker)
{
	if (attacker->body != nullptr){
		if (attacker->body->slots.at(RIGHTHAND) != nullptr){
			return attacker->body->slots.at(RIGHTHAND)->weapon->damageType;
		}
	}
	return NODAMAGETYPE;
}

bool CombatSystem::isResistantToDamageType(GameObject* defender, DamageTypes type)
{
	if (type == NODAMAGETYPE) { return false; }

	if (defender->armour != nullptr){
		if (defender->armour->resistance == type){
			return true;
		} else {
			return false;
		}
	} else {
		if (defender->body->slots.at(BODY) != nullptr) {
			if (defender->body->slots.at(BODY)->armour->resistance == type) {
				return true;
			}
		}
		return false;
	}	
}

bool CombatSystem::isWeakToDamageType(GameObject* defender, DamageTypes type)
{
	if (type == NODAMAGETYPE) { return false; }

	if (defender->armour != nullptr){
		if (defender->armour->weakness == type){
			return true;
		} else {
			return false;
		}
	} else {
		if (defender->body->slots.at(BODY) != nullptr) {
			if (defender->body->slots.at(BODY)->armour->weakness == type) {
				return true;
			}
		}
		return false;
	}
}

void CombatSystem::onDead(DeadEvent event)
{
	if (event.m_uid != 0) {
		for (int i = 0; i <= static_cast<int>(BLEEDING); ++i) {
			m_entities->at(event.m_uid)->statusContainer->statuses.at(static_cast<StatusTypes>(i)).first = 0;
			m_entities->at(event.m_uid)->statusContainer->statuses.at(static_cast<StatusTypes>(i)).second = 0;
		}

		m_eventManager->pushEvent(ExpGainEvent(0, m_entities->at(event.m_uid)->ai->exp));

		m_entities->at(event.m_uid)->fighter->isAlive = false;
		m_entities->at(event.m_uid)->ai = nullptr;
		m_entities->at(event.m_uid)->animation = nullptr;
	}
}

void CombatSystem::onTick()
{
	int dmg;
	std::map<int, GameObject*>::iterator it;
	for (it = m_entities->begin(); it != m_entities->end(); ++it){
		if (it->second->fighter == nullptr) { continue; }
		if (it->second->statusContainer == nullptr){ continue; }

		for (int i = 0; i <= static_cast<int>(BLEEDING); ++i){
			if (it->second->statusContainer->statuses.at(static_cast<StatusTypes>(i)).first > 0){
				it->second->statusContainer->statuses.at(static_cast<StatusTypes>(i)).first -= 1;
				if (it->second->statusContainer->statuses.at(static_cast<StatusTypes>(i)).first == 0){
					it->second->statusContainer->statuses.at(static_cast<StatusTypes>(i)).second = 0;
				}
				dmg = it->second->statusContainer->statuses.at(static_cast<StatusTypes>(i)).second;
				if (dmg > 0){
					m_eventManager->pushEvent(DamageEvent(it->first, dmg));
				}
			}
		}
	}
}

void CombatSystem::update(Uint32 dt)
{
	std::map<int, GameObject*>::iterator it;
	for (it = m_entities->begin(); it != m_entities->end(); ++it){
		if (it->second->fighter != nullptr) {
			if (!it->second->fighter->isAlive) {
				SDL_Color c = { 0xda, 0x24, 0x24 };
				it->second->renderable->colour = c;
				it->second->renderable->chr = '%';

				it->second->renderable->spriteX = 0;
				it->second->renderable->spriteY = 3;
				it->second->renderable->sheet = 20;

				it->second->fighter = nullptr;
			}
		}
	}

}

// notify overrides below

void CombatSystem::notify(AttackEvent event)
{
  doAttack(event);
}

void CombatSystem::notify(OnHitEvent event)
{
  calculateDamage(event);
}

void CombatSystem::notify(OnCriticalHitEvent event)
{
  calculateDamage(event);
}

void CombatSystem::notify(DamageEvent event)
{
  applyDamage(event);
}

void CombatSystem::notify(DeadEvent event)
{
  onDead(event);
}

