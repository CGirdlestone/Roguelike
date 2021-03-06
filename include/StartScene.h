#ifndef STARTSCENE_H
#define STARTSCENE_H

#include "SDL.h"
#include "Renderer.h"
#include "KeyPressSurfaces.h"
#include "EventManager.h"
#include "Scene.h"

class StartScene :public Scene
{
public:
	StartScene(EventManager *eventManager, Renderer *renderer);
	virtual ~StartScene();
	enum KeyPressSurfaces getEvent(SDL_Event* e);
	void render();
	void handleInput(KeyPressSurfaces keyPress);
	void update(Uint32 dt);
	void onTick();
	void reset(){m_i = 0; m_options = 2;};

private:
	EventManager *m_eventManager;
	Renderer *m_renderer;
	int m_i;
	int m_options;
	bool hasSaveFile;
};

#endif
