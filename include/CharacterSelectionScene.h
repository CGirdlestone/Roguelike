#ifndef CHARACTER_SELECTION_H
#define CHARACTER_SELECTION_H

#include <string>
#include <vector>

#include "SDL.h"
#include "Renderer.h"
#include "KeyPressSurfaces.h"
#include "EventManager.h"
#include "Scene.h"

class CharacterSelectionScene : public Scene
{
public:
	CharacterSelectionScene(EventManager* eventManager, Renderer* renderer);
	virtual ~CharacterSelectionScene();

	enum State {TEXT, ATTRIBUTES, CLASS, RACE, DONE};

	enum KeyPressSurfaces getEvent(SDL_Event* e);
	void render();
	void handleInput(KeyPressSurfaces keyPress);
	void processEntities();
	void update(Uint32 dt);
	void onTick();
	void reset();
	void setNextAttributeRolls();
	void backspace();
	void randomCharacter();

private:
	EventManager* m_eventManager{ nullptr };
	Renderer* m_renderer{ nullptr };
	bool character_complete{ false };
	std::vector<std::string> attributes;

	int name_length_limit{ 10 };
	bool text_input{ true };
	std::string player_name{ "" };
	int player_name_index{ 0 };

	bool attribute_selection{ false };
	std::vector<int> player_attributes{ 0, 0, 0, 0, 0, 0 };
	int player_attribute_index{ 0 };
	std::vector<int> attribute_rolls;
	int attribute_index{ 0 };

	bool attributes_selected{ false };
	CharacterSelectionScene::State _state{ TEXT };
};

#endif // !CHARACTER_SELECTION_H
