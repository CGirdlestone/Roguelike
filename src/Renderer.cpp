
#include <iostream>
#include <string>
#include <map>
#include "Renderer.h"
#include "Slots.h"

Renderer::Renderer(Console* console)
{
	m_console = console;
	m_defaultColour = {0x3a, 0x4b, 0x6d};
	m_inViewColour = {0xb4, 0xc3, 0xa8};
	m_borderColour = {0x99, 0xb4, 0xdd};
	m_highlightColour = {0xd3, 0xea, 0xd8};
	m_backgroundColour = {0x07, 0x1b, 0x2c};
	m_textColour = {0x94, 0xa5, 0xaa};
}

Renderer::~Renderer()
{
	m_console = nullptr;
}

void Renderer::toggleAsciiMode()
{
	m_console->toggleAsciiMode();
}

void Renderer::drawObject(GameObject* entity, int x, int y)
{
	if (m_console->getDisplayAscii()){	
  		m_console->render(&entity->renderable->chr, x, y, entity->renderable->colour);
	} else {
		m_console->renderSprite(x, y, entity->renderable->spriteX, entity->renderable->spriteY, entity->renderable->sheet);
	}
}

void Renderer::drawTile(char* c, int x, int y, bool inView)
{
	if (m_console->getDisplayAscii()){
		m_console->render(c, x, y, inView ? m_inViewColour : m_defaultColour);
	} else {
		if ((*c) == '#'){
			m_console->renderSprite(x, y, 1, inView ? 10 : 13, 16);
		} else if ((*c) == '.'){
			m_console->renderSprite(x, y, 1, inView ? 16 : 25, 16);
		}
	}
}

void Renderer::drawLog(MessageLog* messageLog, int height)
{
	const std::vector<Message>& messages = messageLog->getMessages();

	if (messages.size() > 0){
		for(int j = 0; j < static_cast<int>(messages.size()); j++){
			Message msg = messages.at(j);
			for(int i = 0; i < static_cast<int>(msg.m_msg.length()); i++){
				m_console->render(&msg.m_msg[i], i + 1, j + height+1, msg.m_colour);
			}
		}
	}
}

void Renderer::drawMap(Camera* camera, DungeonGenerator* dungeon, std::map<int, GameObject*> *actors)
{
	int x;
	int y = 0;
	int offsetI{ 0 };
	bool occupied{ false };

	for (int i = 0; i < dungeon->Getm_width() * dungeon->Getm_height(); ++i){
		x = i % dungeon->Getm_width();

		if (!(x >= camera->getX() && x < camera->getX() + camera->getWidth() && y >= camera->getY() && y < camera->getY() + camera->getHeight())){
			if (x == dungeon->Getm_width() - 1){
			y++;
			}
			continue;
		}

		offsetI = camera->calculateOffset(x, y);
		occupied = false;
		if (dungeon->m_fovMap[i] == 1){
			
			std::map<int, GameObject*>::iterator it;
			for(it = actors->begin(); it != actors->end(); ++it){
				if (it->second->position == nullptr){ continue; }

				if (it->second->position->x + it->second->position->y*dungeon->Getm_width() == i){
					occupied = true;
				}
			}

			if(!occupied){
				drawTile(&dungeon->m_level[i], offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), true);
			}

			// draw the tile behind an object if the game is in tile display mode
			if (!m_console->getDisplayAscii()){
				drawTile(&dungeon->m_level[i], offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), true);
			}
			
		} else if (dungeon->m_fovMap[i] == 0){
			if (dungeon->m_exploredMap[i] == 1){
				drawTile(&dungeon->m_level[i], offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), false);
			}
		}
		if (x == dungeon->Getm_width() - 1){
			++y;
		}
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
				drawObject(it->second, offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer());
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
	
	std::string health = "Health: " + std::to_string(player->fighter->health) + " / " + std::to_string(player->fighter->maxHealth);
	drawText(health, width + 1, 2 * yPosition++, false);

	SDL_Color colour = { 0xd0, 0x46, 0x48 };
	drawBar(width + 1, 2 * yPosition++, buffer, player->fighter->health, player->fighter->maxHealth, colour);

	std::string exp = "Exp: " + std::to_string(player->player->exp) + " / " + std::to_string(player->player->next);
	drawText(exp, width + 1, 2 * yPosition++, false);

	colour = { 0x59, 0x7d, 0xce };
	drawBar(width + 1, 2 * yPosition++, buffer, player->player->exp, player->player->next, colour);
	
	std::string level = "Level: " + std::to_string(player->player->level);
	drawText(level, width + 1, 2 * yPosition++, false);

	std::string depth = "Depth: " + std::to_string(dungeon->m_uid);
	drawText(depth, width + 1, 2 * yPosition++, false);
}

void Renderer::drawStartMenu(int i, int options)
{
	int width{ m_console->Getm_width() + m_console->getXBuffer() };
	int height{ m_console->Getm_height() + m_console->getYBuffer() };
	std::string name = "ROGUELIKE";
	std::string startText = "START GAME";
	std::string exitText = "EXIT GAME";
	std::string continueText = "CONTINUE";

	int yPosition{ height / 2 - 4 };

	if ((options == 2 && i == 0) || (options == 3 && i == 1)){
		startText = ">" + startText;
	} else if ((options == 2 && i == 1) || (options == 3 && i == 2)){
		exitText = ">" + exitText;
	} else {
		continueText = ">" + continueText;
	}

	m_console->flush();

	drawText(name, width / 2 - 5, yPosition, false);
	yPosition += 2;

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

	std::string inventoryHeader = "Inventory";

	drawText(inventoryHeader, 3, 2, false);

	if (actors->at(0)->inventory->inventory.size() > 0){
		for (int k = 0; k < static_cast<int>(actors->at(0)->inventory->inventory.size());++k){
			GameObject* item = actors->at(0)->inventory->inventory.at(k);
			if (k == i){
				std::string selectedItem = ">" + item->m_name;
				drawText(selectedItem, 2, 2 * k + 4, true);
				std::vector<std::string> lines = wrapText(item->item->description, width / 2 + xBuffer);
				int j = 0;
				for (auto& line : lines) {
					drawText(line, width / 2, 2 + 2*j++, false);
				}
			} else {
				drawText(item->m_name, 3, 2 * k + 4, false);
			}
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
			m_console->render(&equipmentSlot[j], j + 2, 2 + 2 * y, m_highlightColour);
		} else {
			m_console->render(&equipmentSlot[j], j + 3, 2 + 2 * y, m_textColour);
		}	
	}
}

void Renderer::drawCharacterScene(std::map<int, GameObject*> *actors, int index)
{
	m_console->flush();

	GameObject* player = actors->at(0);
	std::string character = "Equipped Items";
	std::string slot{ "" };

	drawBox(0, 0, m_console->Getm_width() + m_console->getXBuffer(), m_console->Getm_height() + m_console->getYBuffer());
	drawBox(0, 0, m_console->Getm_width() / 2, m_console->Getm_height() + m_console->getYBuffer());

	drawText(character, 3, 2, false);

	int armour_bonus = 0;

	for (std::map<EquipSlots, GameObject*>::iterator iter = actors->at(0)->body->slots.begin(); iter != actors->at(0)->body->slots.end(); ++iter){

		switch(iter->first){
			case HEAD: slot = "Head: "; break;
			case LEFTHAND: slot = "Left Hand: "; break;
			case RIGHTHAND: slot = "Right Hand: "; break;
			case BODY: slot = "Body: "; break;
			case NECK: slot = "Amulet: "; break;
			case BACK: slot = "Cloak: "; break;
		}

		if (iter->second != nullptr){
			drawEquippedItem(slot, iter->second->m_name, static_cast<int>(iter->first) + 1, index);
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

	std::string power = "Power: " + std::to_string(player->fighter->power);
	drawText(power, x, 2 * yPosition++, false);

	std::string defence = "Defence: " + std::to_string(player->fighter->defence);
	drawText(defence, x, 2 * yPosition++, false);

	std::string armour_bonus_str = "Armour Bonus: " + std::to_string(armour_bonus);
	drawText(armour_bonus_str, x, 2 * yPosition++, false);
	
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
			m_console->fillBackgroundTile(xOrigin+j, yOrigin+h, m_backgroundColour, 255, m_console->getTileSize(), 0, 0);
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
 			m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour);
  		} else {
			SDL_Color colour = {0xd0, 0x46, 0x48};
 			m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour);
		}
	}

	if (splashRadius > 0){
		for (int i = -splashRadius; i <= splashRadius; ++i){
			for (int j = -splashRadius; j <= splashRadius; ++j){
				
				offsetI = camera->calculateOffset(_x-i, _y-j);
							
				if (checkInRange(_x, _y, actors->at(0)->position->x, actors->at(0)->position->y, radius)){
					SDL_Color colour = {0x6d, 0xaa, 0x2b};
 					m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour);
  				} else {
					SDL_Color colour = {0xd0, 0x46, 0x48};
 					m_console->fillBackgroundTile(offsetI % camera->getWidth()+camera->getXBuffer(), offsetI / camera->getWidth()+camera->getYBuffer(), colour);
				}
			}
		} 
	}

	m_console->update();
}

void Renderer::drawParticles(Camera* camera, DungeonGenerator* dungeon, std::vector<Particle> &particles)
{
	SDL_Color colour;
	int offsetI;
	for (auto p : particles){
		colour = {static_cast<uint8_t>(p.red), static_cast<uint8_t>(p.green), static_cast<uint8_t>(p.blue)};
		offsetI = camera->calculateOffset(p.x, p.y);
		if (p.steps % 2 == 0){
			m_console->fillBackgroundTile(0, 0, colour, 255, p.size, (offsetI % camera->getWidth() +camera->getXBuffer()) * m_console->getTileSize(), (offsetI / camera->getWidth() + camera->getYBuffer()) * m_console->getTileSize());
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

void Renderer::drawBox(int x, int y, int width, int height)
{
	int top_left = 218;
	int top_right = 191;
	int bot_left = 192;
	int bot_right = 217;
	int horizontal = 196;
	int vertical = 179;
	int block = 219;

	for (int h = 0; h < height; ++h) {
		for (int j = 0; j < width; ++j) {
			if (j == 0 && h == 0) {
				m_console->render(top_left, x + j, y + h, m_borderColour);
			}
			else if (j == 0 && h == height - 1) {
				m_console->render(bot_left, x + j, y + h, m_borderColour);
			}
			else if (j == width - 1 && h == 0) {
				m_console->render(top_right, x + j, y + h, m_borderColour);
			}
			else if (j == width - 1 && h == height - 1) {
				m_console->render(bot_right, x + j, y + h, m_borderColour);
			}
			else if (j == 0 || j == width - 1) {
				m_console->render(vertical, x + j, y + h, m_borderColour);
			}
			else if (h == 0 || h == height - 1) {
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


std::vector<std::string> Renderer::wrapText(std::string& text, int width)
{
	std::vector<std::string> lines;
	int last_space{ 0 };
	int line_start{ 0 };

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
		}
		else {
			m_console->render(barChar, x + i, y, m_defaultColour);
		}
	}
}
