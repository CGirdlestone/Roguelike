
#include <iostream>
#include <string>
#include <map>
#include "Renderer.h"
#include "Slots.h"
#include "Utils.h"

Renderer::Renderer(Console* console)
{
	m_console = console;
	m_defaultColour = { 0x35, 0x36, 0x58 };
	m_inViewColour = { 0x8b, 0x97, 0xb6 };
	m_borderColour = { 0x28, 0x90, 0xdc };
	m_highlightColour = { 0x5e, 0xe9, 0xe9 };
	m_textColour = { 0xc5, 0xcd, 0xdb };
}

Renderer::~Renderer()
{
	m_console = nullptr;
}

void Renderer::toggleAsciiMode()
{
	m_console->toggleAsciiMode();
}

void Renderer::drawObject(GameObject* entity, int x, int y, int scale)
{
	if (m_console->getDisplayAscii()){	
  		m_console->render(&entity->renderable->chr, x, y, entity->renderable->colour, scale);
	} else {
		m_console->renderSprite(x, y, entity->renderable->spriteX, entity->renderable->spriteY, entity->renderable->sheet, scale);
	}
}

void Renderer::drawTile(char* c, int x, int y, bool inView, int scale)
{
	if (m_console->getDisplayAscii()){
		m_console->render(c, x, y, inView ? m_inViewColour : m_defaultColour, scale);
	} else {
		if ((*c) == '#'){
			//m_console->renderSprite(x, y, 1, inView ? 10 : 13, 16);
			m_console->renderSprite(x, y, 0, 0, 20, scale);
		} else if ((*c) == '.'){
			//m_console->renderSprite(x, y, 1, inView ? 16 : 25, 16);
			m_console->renderSprite(x, y, 0, 1, 20, scale);
		}
		if (!inView) {
			SDL_Color shadow = { 0x00, 0x00, 0x00 };
			m_console->fillBackgroundTile(x, y, shadow, scale);
		}
	}
}

void Renderer::drawLog(MessageLog* messageLog, int height)
{
	const std::vector<Message> messages = messageLog->getMessages();
	const int log_size = static_cast<int>(messages.size());

	if (messages.size() > 0){
		for(int j = log_size - 1; j >= 0; j--){
			Message msg = messages.at(j);
			drawText(msg.m_msg, 1, (log_size - j) + height, msg.m_colour);
		}
	}
}

void Renderer::autoTile(int x, int y, int i, int offsetI, int tile, Camera* camera, DungeonGenerator* dungeon)
{
	int screenx{ offsetI % camera->getWidth() + camera->getXBuffer() };
	int screeny{ offsetI / camera->getWidth() + camera->getYBuffer() };

	if (dungeon->m_level[x + (y + 1) * dungeon->Getm_width()] == '.') {
		if (dungeon->m_level[x + 1 + (y)*dungeon->Getm_width()] == '.' && (dungeon->m_level[x - 1 + (y)*dungeon->Getm_width()]) == '.') {
			m_console->renderSprite(screenx, screeny, 6, 0, 20, camera->getZoom());
		}
		else if (dungeon->m_level[x + 1 + (y)*dungeon->Getm_width()] == '.') {
			m_console->renderSprite(screenx, screeny, 5, 0, 20, camera->getZoom());
		}
		else if (dungeon->m_level[x - 1 + (y)*dungeon->Getm_width()] == '.') {
			m_console->renderSprite(screenx, screeny, 4, 0, 20, camera->getZoom());
		}
		else {
			m_console->renderSprite(screenx, screeny, dungeon->m_tiles[i], 0, 20, camera->getZoom());
		}
	}
	else if (dungeon->m_level[x + (y - 1) * dungeon->Getm_width()] == '.') {
		if (dungeon->m_level[x + 1 + (y)*dungeon->Getm_width()] == '.' && (dungeon->m_level[x - 1 + (y)*dungeon->Getm_width()]) == '.') {
			m_console->renderSprite(screenx, screeny, 19, 0, 20, camera->getZoom());
		}
		else if (dungeon->m_level[x + 1 + (y)*dungeon->Getm_width()] == '.') {
			m_console->renderSprite(screenx, screeny, 17, 0, 20, camera->getZoom());
		}
		else if (dungeon->m_level[x - 1 + (y)*dungeon->Getm_width()] == '.') {
			m_console->renderSprite(screenx, screeny, 16, 0, 20, camera->getZoom());
		}
		else {
			m_console->renderSprite(screenx, screeny, 12 + dungeon->m_tiles[i], 0, 20, camera->getZoom());
		}
	}
	else if (dungeon->m_level[x + 1 + (y)*dungeon->Getm_width()] == '.' && (dungeon->m_level[x - 1 + (y)*dungeon->Getm_width()]) == '.') {
		m_console->renderSprite(screenx, screeny, 18, 0, 20, camera->getZoom());
	}
	else if (dungeon->m_level[x + 1 + (y)*dungeon->Getm_width()] == '.') {
		m_console->renderSprite(screenx, screeny, 7, 0, 20, camera->getZoom());
	}
	else if (dungeon->m_level[x - 1 + (y)*dungeon->Getm_width()] == '.') {
		m_console->renderSprite(screenx, screeny, 8, 0, 20, camera->getZoom());
	}
}

void Renderer::drawTileMap(Camera* camera, DungeonGenerator* dungeon)
{
	int x{ 0 };
	int y{ 0 };
	int offsetI{ 0 };
	SDL_Color shadow = { 0x00, 0x00, 0x00 };

	for (int i = 0; i < dungeon->Getm_width() * dungeon->Getm_height(); ++i) {
		x = i % dungeon->Getm_width();

		if (!(x >= camera->getX() && x < camera->getX() + camera->getWidth() / camera->getZoom() && y >= camera->getY() && y < camera->getY() + camera->getHeight() / camera->getZoom())) {
			if (x == dungeon->Getm_width() - 1) {
				y++;
			}
			continue;
		}

		offsetI = camera->calculateOffset(x, y);

		int tile = dungeon->m_tiles[i];

		if (dungeon->m_fovMap[i] == 1) {
			if (dungeon->m_level[i] == '#') {
				autoTile(x, y, i, offsetI, tile, camera, dungeon);
			}
			else if (dungeon->m_level[i] == '.') {
				m_console->renderSprite(offsetI % camera->getWidth() + camera->getXBuffer(), offsetI / camera->getWidth() + camera->getYBuffer(), dungeon->m_tiles[i], 1, 20, camera->getZoom());
			}
		}
		else if (dungeon->m_fovMap[i] == 0){
			if (dungeon->m_exploredMap[i] == 1) {
				if (dungeon->m_level[i] == '#') {
					autoTile(x, y, i, offsetI, tile, camera, dungeon);
				}
				else if (dungeon->m_level[i] == '.') {
					m_console->renderSprite(offsetI % camera->getWidth() + camera->getXBuffer(), offsetI / camera->getWidth() + camera->getYBuffer(), dungeon->m_tiles[i], 1, 20, camera->getZoom());
					m_console->fillBackgroundTile(offsetI % camera->getWidth() + camera->getXBuffer(), offsetI / camera->getWidth() + camera->getYBuffer(), shadow, camera->getZoom());
				}
				
			}
		}

		if (x == dungeon->Getm_width() - 1) {
			++y;
		}
	}
}

void Renderer::drawAsciiMap(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*>* actors)
{
	int x;
	int y = 0;
	int offsetI{ 0 };
	bool occupied{ false };

	for (int i = 0; i < dungeon->Getm_width() * dungeon->Getm_height(); ++i) {
		x = i % dungeon->Getm_width();

		if (!(x >= camera->getX() && x < camera->getX() + camera->getWidth() / camera->getZoom() && y >= camera->getY() && y < camera->getY() + camera->getHeight() / camera->getZoom())) {
			if (x == dungeon->Getm_width() - 1) {
				y++;
			}
			continue;
		}

		offsetI = camera->calculateOffset(x, y);
		occupied = false;
		if (dungeon->m_fovMap[i] == 1) {

			std::map<int, GameObject*>::iterator it;
			for (it = actors->begin(); it != actors->end(); ++it) {
				if (it->second->position == nullptr) { continue; }

				if (it->second->position->x + it->second->position->y * dungeon->Getm_width() == i) {
					occupied = true;
				}
			}

			if (!occupied) {
				drawTile(&dungeon->m_level[i], offsetI % camera->getWidth() + camera->getXBuffer(), offsetI / camera->getWidth() + camera->getYBuffer(), true, camera->getZoom());
			}

		}
		else if (dungeon->m_fovMap[i] == 0) {
			if (dungeon->m_exploredMap[i] == 1) {
				drawTile(&dungeon->m_level[i], offsetI % camera->getWidth() + camera->getXBuffer(), offsetI / camera->getWidth() + camera->getYBuffer(), false, camera->getZoom());
			}
		}
		if (x == dungeon->Getm_width() - 1) {
			++y;
		}
	}
}

void Renderer::drawMap(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors)
{
	if (!m_console->getDisplayAscii()) {
		drawTileMap(camera, dungeon);
	}
	else {
		drawAsciiMap(camera, dungeon, actors);
	}
	
}

void Renderer::drawMiniMap(DungeonGenerator* dungeon, std::map<int, GameObject*> *actors)
{
	drawBox(m_console->Getm_width(), m_console->Getm_height(), m_console->getXBuffer(), m_console->getYBuffer());

	int mapOriginX = (m_console->Getm_width() + 2) * m_console->getTileSize() + m_console->getTileSize() / 2;
	int mapOriginY = (m_console->Getm_height() + 1) * m_console->getTileSize();

	SDL_Color player = {0x00, 0x00, 0x00};
	SDL_Color enemy = {0xd0, 0x46, 0x48};
	SDL_Color item = {0x34, 0x65, 0x24};
	SDL_Color other = {0x22, 0x20, 0x34};

	SDL_Color lightWall = {0x75, 0x71, 0x61};
	SDL_Color darkWall = {0x4e, 0x4a, 0x4e};

	int size{ 2 };
	int x{ 0 };
	int y{ 0 };

	for (int i = 0; i < dungeon->Getm_width() * dungeon->Getm_height(); ++i){
		if (dungeon->m_level[i] == ' '){ continue; }	
		if (dungeon->m_exploredMap[i] == 0){ continue; }

		x = i % dungeon->Getm_width();
		y = i / dungeon->Getm_width();
		char tile = dungeon->m_level[i];
		
		if (dungeon->m_fovMap[i] == 0){
			m_console->fillBackgroundTile(x, y, tile == '#' ? darkWall : m_defaultColour, 255, size, mapOriginX, mapOriginY);
		} else if (dungeon->m_fovMap[i] == 1){
			m_console->fillBackgroundTile(x, y, tile == '#' ? lightWall : m_inViewColour, 255, size, mapOriginX, mapOriginY);
		}
	}
	
	std::map<int, GameObject*>::iterator it;
	for (it = actors->begin(); it != actors->end(); ++it){
		if (it->second->position == nullptr){ continue; }

		if (dungeon->m_fovMap[it->second->position->x + it->second->position->y * dungeon->Getm_width()] == 0){ continue; }		

		if (it->first == 0){
			m_console->fillBackgroundTile(it->second->position->x, it->second->position->y, player, 255, size, mapOriginX, mapOriginY);
		} else if (it->second->item != nullptr){
			m_console->fillBackgroundTile(it->second->position->x, it->second->position->y, item, 255, size, mapOriginX, mapOriginY);
		} else if (it->second->ai != nullptr){
			m_console->fillBackgroundTile(it->second->position->x, it->second->position->y, enemy, 255, size, mapOriginX, mapOriginY);
		} else {
			m_console->fillBackgroundTile(it->second->position->x, it->second->position->y, other, 255, size, mapOriginX, mapOriginY);
		}
	}
}

void Renderer::drawActors(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors)
{
	int mapArrayIndex{ 0 };
	int offsetI{ 0 };

	if (!actors->empty()){
		std::map<int, GameObject*>::reverse_iterator it;
		for (it = actors->rbegin(); it != actors->rend(); ++it){
			if (it->second->position == nullptr){ continue; }

			mapArrayIndex = it->second->position->x + it->second->position->y*dungeon->Getm_width();
			if (dungeon->m_fovMap[mapArrayIndex] == 1){
				offsetI = camera->calculateOffset(it->second->position->x, it->second->position->y);

				int x{ it->second->position->x };
				int y{ it->second->position->y };

				if (!(x >= camera->getX() && x < camera->getX() + camera->getWidth() / camera->getZoom() && y >= camera->getY() && y < camera->getY() + camera->getHeight() / camera->getZoom())) {
					continue;
				}

				drawObject(it->second, offsetI % camera->getWidth() + camera->getXBuffer(), offsetI / camera->getWidth() + camera->getYBuffer(), camera->getZoom());
			}
		}
	}
}

void Renderer::drawMenuOutline()
{
	int xBuffer = m_console->getXBuffer();
	int yBuffer = m_console->getYBuffer();
	int height = m_console->Getm_height();
	int width = m_console->Getm_width();

	drawBox(0, 0, width + xBuffer, height + yBuffer);
	drawBox(0, height, width+1, yBuffer);
	drawBox(width, 0, xBuffer, height+yBuffer);
}

void Renderer::drawUI()
{
  drawMenuOutline();
}

void Renderer::drawPlayerInfo(GameObject* player, DungeonGenerator* dungeon)
{
	int width = m_console->Getm_width();
	int buffer = m_console->getXBuffer()-2;
	int yPosition{ 1 };
	
	SDL_Color colour = { 0xd0, 0x46, 0x48 };
	drawBar(width + 1, 2 * yPosition, buffer, player->fighter->health, player->fighter->maxHealth, colour);

	std::string health = "Health: " + std::to_string(player->fighter->health) + " / " + std::to_string(player->fighter->maxHealth);
	drawText(health, width + 1, 2 * yPosition++, false);

	colour = { 0x59, 0x7d, 0xce };
	drawBar(width + 1, 2 * yPosition, buffer, player->player->exp, player->player->next, colour);

	std::string exp = "Exp: " + std::to_string(player->player->exp) + " / " + std::to_string(player->player->next);
	drawText(exp, width + 1, 2 * yPosition++, false);
	
	std::string level = "Level: " + std::to_string(player->player->level);
	drawText(level, width + 1, 2 * yPosition++, false);

	std::string depth = "Depth: " + std::to_string(dungeon->m_uid);
	drawText(depth, width + 1, 2 * yPosition++, false);
}

void Renderer::drawStartMenu(int i, int options)
{
	int width{ m_console->Getm_width() + m_console->getXBuffer() };
	int height{ m_console->Getm_height() + m_console->getYBuffer() };
	std::string startText = "START GAME";
	std::string exitText = "EXIT GAME";
	std::string continueText = "CONTINUE";

	int yPosition{ height / 4 - 4 };

	if ((options == 2 && i == 0) || (options == 3 && i == 1)){
		startText = ">" + startText;
	} else if ((options == 2 && i == 1) || (options == 3 && i == 2)){
		exitText = ">" + exitText;
	} else {
		continueText = ">" + continueText;
	}

	m_console->flush();

	m_console->renderImage(21);

	if (options == 3){
		
    	if (i == 0){
      		drawText(continueText, width/2 - 6, yPosition, true);
    	} else {
			drawText(continueText, width / 2 - 5, yPosition, false);
    	}
		yPosition += 2;
	}

	if ((options == 3 && i == 1) || (options == 2 && i == 0)){
		drawText(startText, width / 2 - 6, yPosition, true);
	} else {
		drawText(startText, width / 2 - 5, yPosition, false);
	}
	yPosition += 2;

	if ((options == 3 && i == 2) || (options == 2 && i == 1)){
		drawText(exitText, width / 2 - 6, yPosition, true);
	} else {
		drawText(exitText, width / 2 - 5, yPosition, false);
	}

	m_console->update();
}

void Renderer::drawInventory(std::map<int, GameObject*> *actors, int i)
{
	int xBuffer = m_console->getXBuffer();
	int yBuffer = m_console->getYBuffer();
	int height = m_console->Getm_height();
	int width = m_console->Getm_width();

	m_console->flush();
	
	drawBox(0, 0, width + xBuffer, height + yBuffer);
	drawBox(0, 0, width / 2, height + yBuffer);
	drawBox(width / 2 - 1, 0, width / 2 + xBuffer + 1, 8);
	drawBox(width / 2 - 1, 7, width / 2 + xBuffer + 1, height + yBuffer - 7);

	std::string inventoryHeader = ">Inventory";
	std::string characterHeader = "Character";
	SDL_Colour colour = { 0x35, 0x36, 0x58 };

	drawBar(1, 1, width / 2 - 2, width / 2 - 2, width / 2 - 2, colour);
	drawText(inventoryHeader, 2, 1, true);
	drawText(characterHeader, 2 + inventoryHeader.length() + 2, 1, false);

	int yPosition{ 5 };

	if (actors->at(0)->inventory->inventory.size() > 0){
		for (int k = 0; k < static_cast<int>(actors->at(0)->inventory->inventory.size());++k){
			GameObject* entity = actors->at(0)->inventory->inventory.at(k);
			if (k == i){

				std::string selectedItem = ">" + entity->m_name;

				drawText(selectedItem, 2, yPosition, true);

				std::vector<std::string> lines = wrapText(entity->item->description, width / 2 + xBuffer);
				int j = 0;
				for (auto& line : lines) {
					drawText(line, width / 2, 2 + 2*j++, false);
				}

				if (m_console->getDisplayAscii()) {
					m_console->render(entity->renderable->chr, width / 2, 8, entity->renderable->colour, 8, false);
				}
				else {
					m_console->renderSprite(width / 2, 8, entity->renderable->spriteX, entity->renderable->spriteY, entity->renderable->sheet, 8, false);
				}
			} else {
				drawText(entity->m_name, 3, yPosition, false);
			}
			yPosition += 2;
		}
	}

	m_console->update();
}

void Renderer::drawEquippedItem(std::string slot, std::string item, int y, int index)
{
	std::string equipmentSlot = slot + item;

	if (index == y - 1){
		equipmentSlot = ">" + equipmentSlot;
	}	

	for (int j = 0; j < static_cast<int>(equipmentSlot.length()); ++j){
		if (index == y - 1){
			m_console->render(&equipmentSlot[j], j + 2, 3 + 2 * y, m_highlightColour);
		} else {
			m_console->render(&equipmentSlot[j], j + 3, 3 + 2 * y, m_textColour);
		}	
	}
}

void Renderer::drawCharacterScene(std::map<int, GameObject*> *actors, int index)
{
	m_console->flush();

	int width = m_console->Getm_width();

	std::string inventoryHeader = "Inventory";
	std::string characterHeader = ">Character";
	SDL_Colour colour = { 0x35, 0x36, 0x58 };

	drawBar(1, 1, width / 2 - 2, width / 2 - 2, width / 2 - 2, colour);
	drawText(inventoryHeader, 3, 1, false);
	drawText(characterHeader, 2 + inventoryHeader.length() + 2, 1, true);

	GameObject* player = actors->at(0);
	std::string slot{ "" };

	drawBox(0, 0, m_console->Getm_width() + m_console->getXBuffer(), m_console->Getm_height() + m_console->getYBuffer());
	drawBox(0, 0, m_console->Getm_width() / 2, m_console->Getm_height() + m_console->getYBuffer());

	int size{ 10 };
	int row{ 17 };
	int col{ 3 };

	drawBox(col, row, size, size);
	drawBox(col + size + 1, row, size, size);
	drawBox(col + 2 * (size + 1), row, size, size);

	drawBox(col, row + size + 1, size, size);
	drawBox(col + size + 1, row + size + 1, size, size);
	drawBox(col + 2 * (size + 1), row + size + 1, size, size);

	int x_pos[6] = { 15, 4, 26, 15, 4, 26 };
	int y_pos[6] = { 18, 29, 29, 29, 18, 18 };

	int armour_bonus = 0;

	for (std::map<EquipSlots, GameObject*>::iterator iter = actors->at(0)->body->slots.begin(); iter != actors->at(0)->body->slots.end(); ++iter){

		switch(iter->first){
			case HEAD: slot =		"Head:       "; break;
			case LEFTHAND: slot =	"Left Hand:  "; break;
			case RIGHTHAND: slot =	"Right Hand: "; break;
			case BODY: slot =		"Body:       "; break;
			case NECK: slot =		"Amulet:     "; break;
			case BACK: slot =		"Cloak:      "; break;
		}

		if (iter->second != nullptr){
			drawEquippedItem(slot, iter->second->m_name, static_cast<int>(iter->first) + 1, index);
			m_console->renderSprite(x_pos[iter->first], y_pos[iter->first], iter->second->renderable->spriteX, iter->second->renderable->spriteY, iter->second->renderable->sheet, 8, false);
			if (iter->second->armour != nullptr) {
				armour_bonus += iter->second->armour->armourBonus;
			}
		} else {
			std::string empty = "empty";
			drawEquippedItem(slot, empty, static_cast<int>(iter->first) + 1, index);
		}
	}

	int yPosition = 1;
	int x = m_console->Getm_width() / 2 + 1;

	std::string level = "Level: " + std::to_string(player->player->level);
	drawText(level, x, 2 * yPosition++, false);

	std::string exp = "Exp: " + std::to_string(player->player->exp) + " / " + std::to_string(player->player->next);
	drawText(exp, x, 2 * yPosition++, false);

	std::string health = "Health: " + std::to_string(player->fighter->health) + " / " + std::to_string(player->fighter->maxHealth);
	drawText(health, x, 2 * yPosition++, false);

	std::string ac = "AC: " + std::to_string(player->fighter->armour_class);
	drawText(ac, x, 2 * yPosition++, false);

	int mod = utils::getAttributeMod(player->fighter->strength);
	std::string attrib_mod = (mod < 0 ? std::to_string(mod).substr(1, std::to_string(mod).length()) : std::to_string(mod));
	std::string attrib = (player->fighter->strength < 10 ?  (mod < 0 ? "  (-" : "  (+") : (mod < 0 ? " (-" : " (+" ));
	std::string strength = "Strength:     " + std::to_string(player->fighter->strength) + attrib + attrib_mod + ")";
	drawText(strength, x, 2 * yPosition++, false);

	mod = utils::getAttributeMod(player->fighter->dexterity);
	attrib_mod = (mod < 0 ? std::to_string(mod).substr(1, std::to_string(mod).length()) : std::to_string(mod));
	attrib = (player->fighter->dexterity < 10 ? (mod < 0 ? "  (-" : "  (+") : (mod < 0 ? " (-" : " (+"));
	std::string dexterity = "Dexterity:    " + std::to_string(player->fighter->dexterity) + attrib + attrib_mod + ")";
	drawText(dexterity, x, 2 * yPosition++, false);

	mod = utils::getAttributeMod(player->fighter->constitution);
	attrib_mod = (mod < 0 ? std::to_string(mod).substr(1, std::to_string(mod).length()) : std::to_string(mod));
	attrib = (player->fighter->constitution < 10 ? (mod < 0 ? "  (-" : "  (+") : (mod < 0 ? " (-" : " (+"));
	std::string constitution = "Constitution: " + std::to_string(player->fighter->constitution) + attrib + attrib_mod + ")";
	drawText(constitution, x, 2 * yPosition++, false);

	mod = utils::getAttributeMod(player->fighter->intelligence);
	attrib_mod = (mod < 0 ? std::to_string(mod).substr(1, std::to_string(mod).length()) : std::to_string(mod));
	attrib = (player->fighter->intelligence < 10 ? (mod < 0 ? "  (-" : "  (+") : (mod < 0 ? " (-" : " (+"));
	std::string intelligence = "Intelligence: " + std::to_string(player->fighter->intelligence) + attrib + attrib_mod + ")";
	drawText(intelligence, x, 2 * yPosition++, false);

	mod = utils::getAttributeMod(player->fighter->wisdom);
	attrib_mod = (mod < 0 ? std::to_string(mod).substr(1, std::to_string(mod).length()) : std::to_string(mod));
	attrib = (player->fighter->wisdom < 10 ? (mod < 0 ? "  (-" : "  (+") : (mod < 0 ? " (-" : " (+"));
	std::string wisdom = "Wisdom:       " + std::to_string(player->fighter->wisdom) + attrib + attrib_mod + ")";
	drawText(wisdom, x, 2 * yPosition++, false);

	mod = utils::getAttributeMod(player->fighter->charisma);
	attrib_mod = (mod < 0 ? std::to_string(mod).substr(1, std::to_string(mod).length()) : std::to_string(mod));
	attrib = (player->fighter->charisma < 10 ? (mod < 0 ? "  (-" : "  (+") : (mod < 0 ? " (-" : " (+"));
	std::string charisma = "Charisma:     " + std::to_string(player->fighter->charisma) + attrib + attrib_mod + ")";
	drawText(charisma, x, 2 * yPosition++, false);

	m_console->update();
}

void Renderer::drawPauseMenu(int index, Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors, MessageLog* messageLog)
{
	m_console->flush();

	drawMap(camera, dungeon, actors);
	drawActors(camera, dungeon, actors);
	drawLog(messageLog, camera->getHeight()+camera->getYBuffer());
	drawUI();
	drawPlayerInfo(actors->at(0), dungeon);
	drawMiniMap(dungeon, actors);

	std::string startText = "CONTINUE";
	std::string exitText = "SAVE & EXIT GAME";

	int xOrigin = (m_console->Getm_width())/2 - 5;
	int yOrigin = (m_console->Getm_height())/2 - 2;
	int boxWidth = startText.size() < exitText.size() ? static_cast<int>(exitText.size()) + 4 : static_cast<int>(startText.size()) + 4;
	int boxHeight = 7;

	for (int h = 0; h < boxHeight; ++h){
		for (int j = 0; j < boxWidth; ++j){
			m_console->fillBackgroundTile(xOrigin+j, yOrigin+h, m_console->getDrawColour(), 255, m_console->getTileSize(), 0, 0);
		}
	}

	drawBox(xOrigin, yOrigin, boxWidth, boxHeight);
	drawText(startText, xOrigin + 2, yOrigin + 2, index == 0 ? true : false);
	drawText(exitText, xOrigin + 2, yOrigin + 4, index == 1 ? true : false);

	m_console->update();
}

void Renderer::drawTargetingScene(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors, MessageLog* messageLog, int radius, std::vector<int> *path, int splashRadius, int _x, int _y)
{
	int offsetI{ 0 }, x{ 0 }, y{ 0 };
	
	m_console->flush();
	drawMap(camera, dungeon, actors);
	drawActors(camera, dungeon, actors);
	drawLog(messageLog, camera->getHeight()+camera->getYBuffer());
	drawUI();
	drawPlayerInfo(actors->at(0), dungeon);
	drawMiniMap(dungeon, actors);

	for (int i = 0; i < static_cast<int>(path->size()); ++i){
		x = path->at(i) % dungeon->Getm_width();
		y = path->at(i) / dungeon->Getm_width();
	
		offsetI = camera->calculateOffset(x, y);

		if (checkInRange(x, y, actors->at(0)->position->x, actors->at(0)->position->y, radius)){
			SDL_Color colour = {0x6d, 0xaa, 0x2c};
 			m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour, camera->getZoom());
  		} else {
			SDL_Color colour = {0xd0, 0x46, 0x48};
 			m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour, camera->getZoom());
		}
	}

	if (splashRadius > 0){
		for (int i = -splashRadius; i <= splashRadius; ++i){
			for (int j = -splashRadius; j <= splashRadius; ++j){
				
				offsetI = camera->calculateOffset(_x-i, _y-j);
							
				if (checkInRange(_x, _y, actors->at(0)->position->x, actors->at(0)->position->y, radius)){
					SDL_Color colour = {0x6d, 0xaa, 0x2b};
 					m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour, camera->getZoom());
  				} else {
					SDL_Color colour = {0xd0, 0x46, 0x48};
 					m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour, camera->getZoom());
				}
			}
		} 
	}

	m_console->update();
}

void Renderer::drawParticles(Camera* camera, DungeonGenerator* dungeon, std::vector<Particle> &particles)
{
	int offsetI{ 0 };

	for (auto p : particles){
		offsetI = camera->calculateOffset((int)p.x, (int)p.y);
		int x{ offsetI % camera->getWidth() + camera->getXBuffer() };
		int y{ offsetI / camera->getWidth() + camera->getYBuffer() };

		if (m_console->getDisplayAscii()) {
			m_console->render(p.renderable.chr, x, y, p.renderable.colour, camera->getZoom());
		}
		else {
			m_console->renderSprite(x, y, p.renderable.spriteX, p.renderable.spriteY, p.renderable.sheet, camera->getZoom());
		}
	}
}

void Renderer::drawGameScreen(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors, MessageLog* messageLog, std::vector<Particle> &particles)
{
	m_console->flush();
	drawMap(camera, dungeon, actors);
	drawActors(camera, dungeon, actors);
	drawLog(messageLog, camera->getHeight()+camera->getYBuffer());
	drawUI();
	drawPlayerInfo(actors->at(0), dungeon);
	drawMiniMap(dungeon, actors);
	drawParticles(camera, dungeon, particles);
	m_console->update();
}

void Renderer::drawGameOver(int i, std::vector<std::string> &deathMessages, int totalLines)
{
	std::string restart =  i == 0 ? ">Play again" : "Play again";
	std::string exitGame = i == 1 ? ">Exit game" : "Exit game";

	int xPosition = (m_console->Getm_width() + m_console->getXBuffer())/2 - 15;
	int yPosition = (m_console->Getm_height() + m_console->getYBuffer())/2 - 16;

	m_console->flush();

	for (auto & line : deathMessages){
  		for (uint32_t j = 0; j < line.length(); ++j){
    		m_console->render(&line[j], xPosition + j, yPosition, m_textColour);
  		}
		yPosition += 2;
	}

	for (uint32_t i = 0; i < totalLines - deathMessages.size(); ++i){
		yPosition += 2;
	}

	yPosition += 1;
	
	drawText(restart, i == 0 ? xPosition + 10 - 1 : xPosition + 10, yPosition, i == 0 ? true : false);
	yPosition += 2;
	drawText(exitGame, i == 1 ? xPosition + 10 - 1 : xPosition + 10, yPosition, i == 1 ? true : false);

	m_console->update();
}

void Renderer::drawCharacterCreation(const std::vector<int>& attributes, const std::vector<std::string>& attribute_names, std::string player_name, bool text_input, int name_index, int name_size_limit, int attribute_index, const std::vector<int>& attribute_rolls, int current_attribute)
{
	m_console->flush();

	if (player_name.size() < name_size_limit && text_input) {
		player_name += '_';
	}

	drawBox(0, 0, m_console->Getm_width(), m_console->Getm_height());
	
	int y_position{ 1 };

	std::string name_text = "Name: " + player_name;
	drawText(name_text, 2, 2 * y_position++, m_textColour);

	std::string attribute_roll_strings = "Attribute: ";
	drawText(attribute_roll_strings, 2, 2 * y_position, m_textColour);

	int x{ 2 + (int)attribute_roll_strings.size() };
	if (!text_input && !attribute_rolls.empty()) {
		for (int i = 0; i < attribute_rolls.size(); ++i) {
			std::string score = std::to_string(attribute_rolls[i]);
			drawText(score, x, 2 * y_position, i == current_attribute ? true : false);
			x += (int)score.size() + 1;
		}
	}

	y_position++;
	
	for (Uint32 i = 0; i < attributes.size(); ++i) {
		std::string attribute_string = attribute_names[i] + ": " + std::to_string(attributes[i]);
		if (attribute_index == i && !text_input) {
			drawText(attribute_string, 2, 2 * y_position++, m_highlightColour);
		}
		else {
			drawText(attribute_string, 2, 2 * y_position++, m_textColour);
		}
	}

	std::string done = "Finish";
	drawText(done, 2, 2 * y_position++, attribute_index > 5 ? true : false);

	m_console->update();

}

void Renderer::drawBox(int x, int y, int width, int height)
{
	int top_left = 218;
	int top_right = 191;
	int bot_left = 192;
	int bot_right = 217;
	int horizontal = 196;
	int vertical = 179;

	for (int h = 0; h < height; ++h) {
		for (int j = 0; j < width; ++j) {
			if (j == 0 && h == 0) {
				m_console->render(top_left, x + j, y + h, m_borderColour);
			} else if (j == 0 && h == height - 1) {
				m_console->render(bot_left, x + j, y + h, m_borderColour);
			} else if (j == width - 1 && h == 0) {
				m_console->render(top_right, x + j, y + h, m_borderColour);
			} else if (j == width - 1 && h == height - 1) {
				m_console->render(bot_right, x + j, y + h, m_borderColour);
			} else if (j == 0 || j == width - 1) {
				m_console->render(vertical, x + j, y + h, m_borderColour);
			} else if (h == 0 || h == height - 1) {
				m_console->render(horizontal, x + j, y + h, m_borderColour);
			}
		}
	}
}

void Renderer::drawText(std::string& text, int x, int y, bool highlighted)
{
	SDL_Color colour = m_textColour;
	if (highlighted) {
		colour = m_highlightColour;
	}
	for (uint32_t i = 0; i < text.length(); ++i) {
		m_console->render(&text[i], x + i, y, colour);
	}
}

void Renderer::drawText(std::string& text, int x, int y, SDL_Color colour)
{
	for (uint32_t i = 0; i < text.length(); ++i) {
		m_console->render(&text[i], x + i, y, colour);
	}
}

std::vector<std::string> Renderer::wrapText(std::string& text, int width)
{
	std::vector<std::string> lines;
	int last_space{ 0 };
	int line_start{ 0 };

	if (text.length() < width) {
		lines.push_back(text);
		return lines;
	}

	for (uint32_t i = 0; i < text.length(); ++i) {
		if (text[i] == ' ') {
			last_space = i;
		}
		
		if (i == text.length() - 1) {
			lines.push_back(text.substr(line_start, i + 1 - line_start));
			break;
		}

		if (i % width == 0 && i != 0) {
			lines.push_back(text.substr(line_start, last_space - line_start));
			line_start = last_space;
		}
	}
	
	return lines;
}

void Renderer::drawBar(int x, int y, int width , int current, int max, SDL_Color colour)
{
	int barChar = 219;
	int barWidth = (current * width) / max;

	for (int i = 0; i < width; ++i) {
		if (i <= barWidth && barWidth != 0) {
			m_console->render(barChar, x + i, y, colour);
		} else {
			m_console->render(barChar, x + i, y, m_defaultColour);
		}
	}
}

void Renderer::serialise(std::ofstream& file)
{
	utils::serialiseInt(file, m_console->getDisplayAscii() ? 1 : 0);
}

int Renderer::deserialise(char* buffer, int i)
{
	int ascii = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	if (ascii == 0) {
		m_console->toggleAsciiMode();
	}

	return i;
}