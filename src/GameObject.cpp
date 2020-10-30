
#include "GameObject.h"
#include "Components.h"
#include "Utils.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

GameObject::GameObject():
	m_uid(-1)
{
	position = nullptr;
	renderable = nullptr;
	fighter = nullptr;
	actor = nullptr;
	player = nullptr;
	ai = nullptr;
	item = nullptr;
	inventory = nullptr;
	weapon = nullptr;
	armour = nullptr;
	wearable = nullptr;
	body = nullptr;
	useable = nullptr;
	healing = nullptr;
	damage = nullptr;
	areaDamage = nullptr;
	status = nullptr;
	consumable = nullptr;
	stairs = nullptr;
	statusContainer = nullptr;
}

GameObject::~GameObject()
{
	if(position != nullptr){
		delete position;
		position = nullptr;
	}

	if(renderable != nullptr){
		delete renderable;
		renderable = nullptr;
	}

	if(fighter != nullptr){
		delete fighter;
		fighter = nullptr;
	}

	if(actor != nullptr){
		delete actor;
		actor = nullptr;
	}

	if(player != nullptr){
		delete player;
		player = nullptr;
	}

	if(ai != nullptr){
		delete ai;
		ai = nullptr;
	}

	if(item != nullptr){
		delete item;
		item = nullptr;
	}

	if(inventory != nullptr){
		delete inventory;
		inventory = nullptr;
	}

	if (weapon != nullptr) {
		delete weapon;
		weapon = nullptr;
	}
	 
	if(armour != nullptr){
		delete armour;
		armour = nullptr;
	}

	if(wearable != nullptr){
		delete wearable;
		wearable = nullptr;
	}

	if(body != nullptr){
		delete body;
		body = nullptr;
	}
	
	if(useable != nullptr){
		delete useable;
		useable = nullptr;
	}

	if(healing != nullptr){
		delete healing;
		healing = nullptr;
	}

	if(damage != nullptr){
		delete damage;
		damage = nullptr;
	}

	if(areaDamage != nullptr){
		delete areaDamage;
		areaDamage = nullptr;
	}

	if(status != nullptr){
		delete status;
		status = nullptr;
	}

	if(consumable != nullptr){
		delete consumable;
		consumable = nullptr;
	}
	
	if(stairs != nullptr){
		delete stairs;
		stairs = nullptr;
	}

	if(statusContainer != nullptr){
		delete statusContainer;
		statusContainer = nullptr;
	}
}

void GameObject::serialise(std::ofstream& file)
{
	utils::serialiseInt(file, m_uid);

	utils::serialiseString(file, m_name);

	if (position == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		position->serialise(file);
	}

	if (renderable == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		renderable->serialise(file);
	}

	if (fighter == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		fighter->serialise(file);
	}

	if (actor == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		actor->serialise(file);
	}

	if (player == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		player->serialise(file);
	}

	if (ai == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		ai->serialise(file);
	}

	if (item == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		item->serialise(file);
	}

	if (inventory == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		inventory->serialise(file);
	}

	if (weapon == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		weapon->serialise(file);
	}

	if (armour == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		armour->serialise(file);
	}

	if (wearable == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		wearable->serialise(file);
	}

	if (body == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		body->serialise(file);
	}

	if (useable == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		useable->serialise(file);
	}

	if (healing == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		healing->serialise(file);
	}

	if (damage == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		damage->serialise(file);
	}

	if (areaDamage == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		areaDamage->serialise(file);
	}

	if (status == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		status->serialise(file);
	}

	if (consumable == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		consumable->serialise(file);
	}

	if (stairs == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		stairs->serialise(file);
	}

	if (statusContainer == nullptr){
		utils::serialiseInt(file, 0);
	} else {
		utils::serialiseInt(file, 1);
		statusContainer->serialise(file);
	}
}

int GameObject::deserialise(char* buffer, int i, int length)
{
	int numBytes = 4;
	int hasPointer = 0;
	int _nameLength = 0;

	/* Read in the entity's uid */
	m_uid = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	/* Read in the bytes holding the length of the entity's name */
	/*
	_nameLength = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);


	char* name = new char[_nameLength];
	for (int k = 0; k < _nameLength; ++k){
		int letterCode = utils::deserialiseInt(buffer, i);
		i = utils::advanceFourBytes(i);
		name[k] = static_cast<char>(letterCode);
	}
	m_name.assign(name, _nameLength);
	delete[] name;
	*/

	i = utils::deserialiseString(buffer, i, m_name);

	/* Read in the entity's components. The first set of 4 bytes denotes whether the pointer is valid (1) or nullptr (0). */
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	if (hasPointer == 1){
		position = new Position();
		i = position->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	if (hasPointer == 1){
		renderable = new Renderable();
		i = renderable->deserialise(buffer, i);
	}
	hasPointer = 0;

	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	if (hasPointer == 1){
		fighter = new Fighter();
		i = fighter->deserialise(buffer, i);
	}
	hasPointer = 0;

	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		actor = new Actor();
		i = actor->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		player = new Player();
		i = player->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		ai = new AI();
		i = ai->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		item = new Item();
		i = item->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		inventory = new Inventory();
		i = inventory->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		weapon = new Weapon();
		i = weapon->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		armour = new Armour();
		i = armour->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		wearable = new Wearable();
		i = wearable->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		body = new Body();
		i = body->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		useable = new Useable();
		i = useable->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		healing = new Healing();
		i = healing->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		damage = new Damage();
		i = damage->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		areaDamage = new AreaDamage();
		i = areaDamage->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		status = new Status();
		i = status->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		consumable = new Consumable();
		i = consumable->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		stairs = new Stairs();
		i = stairs->deserialise(buffer, i);
	}
	hasPointer = 0;
	
	hasPointer = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	if (hasPointer == 1){
		statusContainer = new StatusContainer();
		i = statusContainer->deserialise(buffer, i);
	}
	hasPointer = 0;

	return i;
}



