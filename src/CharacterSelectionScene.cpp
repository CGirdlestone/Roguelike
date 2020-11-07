#include "CharacterSelectionScene.h"
#include "Utils.h"
#include <iostream>

CharacterSelectionScene::CharacterSelectionScene(EventManager* eventManager, Renderer* renderer):
	m_eventManager(eventManager), m_renderer(renderer)
{
	attributes = { "Strength", "Dexterity", "Constitution", "Intelligence", "Wisdom", "Charisma" };
}

CharacterSelectionScene::~CharacterSelectionScene()
{

}

void CharacterSelectionScene::backspace()
{
	player_name = player_name.substr(0, player_name.length() - 1);
}

enum KeyPressSurfaces CharacterSelectionScene::getEvent(SDL_Event* e)
{
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_QUIT) {
			return ESCAPE;
		}

		else if (e->type == SDL_KEYDOWN) {
			if ((int)e->key.keysym.sym >= 97 && (int)e->key.keysym.sym <= 122) {
				if (text_input) {
					if (player_name.length() < name_length_limit) {
						player_name += static_cast<char>((int)e->key.keysym.sym);
						player_name_index++;
					}
				}
				break;
			}

			switch (e->key.keysym.sym) {
			case SDLK_RETURN:
				if (text_input) {
					text_input = false;
					attribute_selection = true;
					setNextAttributeRoll();
				}
				else {
					if (attribute_selection) {
						if (player_attributes[player_attribute_index] == 0) {
							std::vector<int> new_attribs;
							for (Uint32 i = 0; i < player_attributes.size(); ++i) {
								if (player_attribute_index == i) {
									new_attribs.push_back(current_attribute_roll);
								}
								else {
									new_attribs.push_back(player_attributes.at(i));
								}
							}
							
							player_attributes.swap(new_attribs);

							setNextAttributeRoll();
						}

						int remaining_attributes{ 0 };
						for (int attribute : player_attributes) {
							if (attribute == 0) {
								remaining_attributes++;
							}
						}

						if (remaining_attributes == 0) {
							attributes_selected = true;
							attribute_selection = false;
							player_attribute_index = 6;
						}
					}
					else {
						// selected a name and attributes so pass name and attributes in event.
						PushScene transition = PushScene(GAMESCENE);
						m_eventManager->pushEvent(transition);
						m_eventManager->pushEvent(PassAttributeInfoEvent(player_attributes, player_name));
					}
				}
				break;

			case SDLK_DOWN:
				if (attribute_selection) {
					player_attribute_index++;
					if (attributes_selected) {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 5));
					}
					else {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 6));
					}
				}
				break;

			case SDLK_UP:
				if (attribute_selection) {
					player_attribute_index--;
					if (attributes_selected) {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 5));
					}
					else {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 6));
					}
				}
				break;

			case SDLK_BACKSPACE:
				backspace();
				break;
			}
		}
	}
	return NONE;
}


void CharacterSelectionScene::handleInput(KeyPressSurfaces keyPress)
{

}


void CharacterSelectionScene::setNextAttributeRoll()
{
	current_attribute_roll = utils::roll(3, 6);
}


void CharacterSelectionScene::render()
{
	m_renderer->drawCharacterCreation(player_attributes, attributes, player_name, text_input, player_name_index, name_length_limit, player_attribute_index, current_attribute_roll);
}


void CharacterSelectionScene::update(Uint32 dt)
{

}

void CharacterSelectionScene::onTick()
{

}