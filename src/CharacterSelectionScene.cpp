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

void CharacterSelectionScene::randomCharacter()
{
	player_attributes.clear();
	setNextAttributeRolls();

	for (Uint32 i = 0; i < attribute_rolls.size(); ++i) {
		player_attributes.push_back(attribute_rolls[i]);
	}

	_state = CharacterSelectionScene::DONE;
}

enum KeyPressSurfaces CharacterSelectionScene::getEvent(SDL_Event* e)
{
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_QUIT) {
			return ESCAPE;
		}
		else if (e->type == SDL_KEYDOWN) {
			if (_state == CharacterSelectionScene::TEXT) {
				if ((int)e->key.keysym.sym >= 97 && (int)e->key.keysym.sym <= 122) {
					if (text_input) {
						if (player_name.length() < name_length_limit) {
							player_name += static_cast<char>((int)e->key.keysym.sym);
							player_name_index++;
						}
					}
				}
			}

			switch (e->key.keysym.sym) {
			case SDLK_RETURN:
				switch (_state) {
				case CharacterSelectionScene::TEXT:
					_state = CharacterSelectionScene::ATTRIBUTES;
					text_input = false;
					setNextAttributeRolls();
					break;

				case CharacterSelectionScene::ATTRIBUTES:
					if (player_attributes[player_attribute_index] == 0) {
						std::vector<int> new_attribs;
						for (Uint32 i = 0; i < player_attributes.size(); ++i) {
							if (player_attribute_index == i) {
								new_attribs.push_back(attribute_rolls[attribute_index++]);
							}
							else {
								new_attribs.push_back(player_attributes.at(i));
							}
						}

						player_attributes.swap(new_attribs);
					}

					if (attribute_index == 6) {
						_state = CharacterSelectionScene::DONE;
						attributes_selected = true;
						attribute_selection = false;
						player_attribute_index = 6;
					}
					break;

				case CharacterSelectionScene::CLASS:
					_state = CharacterSelectionScene::RACE;
					break;

				case CharacterSelectionScene::RACE:
					_state = CharacterSelectionScene::DONE;
					break;

				case CharacterSelectionScene::DONE:
					m_eventManager->pushEvent(PassAttributeInfoEvent(player_attributes, player_name));
					m_eventManager->pushEvent(PushScene(GAMESCENE));
					break;
				}
				break;

			case SDLK_DOWN:
				if (_state == CharacterSelectionScene::ATTRIBUTES) {
					player_attribute_index++;
					if (_state != CharacterSelectionScene::DONE) {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 5));
					}
					else {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 6));
					}
				}
				break;

			case SDLK_UP:
				if (_state == CharacterSelectionScene::ATTRIBUTES) {
					player_attribute_index--;
					if (_state != CharacterSelectionScene::DONE) {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 5));
					}
					else {
						player_attribute_index = std::max(0, std::min(player_attribute_index, 6));
					}
				}
				break;

			case SDLK_BACKSPACE:
				if (_state == CharacterSelectionScene::TEXT) {
					backspace();
					break;
				}

			case SDLK_r:
				if (_state == CharacterSelectionScene::ATTRIBUTES || _state == CharacterSelectionScene::DONE){
					randomCharacter();
					player_attribute_index = 6;
					break;
				}
			}
		}
	}
	return NONE;
}


void CharacterSelectionScene::handleInput(KeyPressSurfaces keyPress)
{

}


void CharacterSelectionScene::setNextAttributeRolls()
{
	attribute_rolls.clear();
	for (int i = 0; i < 6; ++i) {
		attribute_rolls.push_back(utils::roll(3, 6));
	}
}


void CharacterSelectionScene::render()
{
	m_renderer->drawCharacterCreation(player_attributes, attributes, player_name, text_input, player_name_index, name_length_limit, player_attribute_index, attribute_rolls, attribute_index);
}


void CharacterSelectionScene::update(Uint32 dt)
{

}

void CharacterSelectionScene::onTick()
{

}