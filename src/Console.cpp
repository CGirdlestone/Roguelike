#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "Console.h"
#include "Tile.h"




Console::Console(int width, int height, const char* title, const char* path, int tileSize)
{
    m_width = width;
    m_height = height;
    m_title = title;
    m_root = nullptr;
    m_renderer = nullptr;
    m_tileSize = tileSize;
    m_xBuffer = 20;
    m_yBuffer = 12;
    m_background = { 0x18, 0x0d, 0x2f };
	displayAscii = true; // default mode is traditional ascii graphics
    init(path);
}

Console::~Console()
{
    SDL_DestroyWindow(m_root);
    m_root = nullptr;

    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;

	for (auto tex : m_spriteSheets){
        SDL_DestroyTexture(tex);
        tex = nullptr;
	}
}

bool Console::init(const char* path)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL Coult not initialise! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if(!createWindow()){
        return false;
    }

    if(!createRenderer()){
        return false;
    }

    if(!initImage()){
        return false;
    }

    loadMedia(path, true);
    createTiles();

	loadMedia("./resources/sprites/Player0.png", true); // 1
	loadMedia("./resources/sprites/Avian0.png", true); // 2
	loadMedia("./resources/sprites/Rodent0.png", true); // 3
	loadMedia("./resources/sprites/Humanoid0.png", true); // 4
	loadMedia("./resources/sprites/Demon0.png", true); // 5
	loadMedia("./resources/sprites/Elemental0.png", true); // 6
	loadMedia("./resources/sprites/Undead0.png", true); // 7
	loadMedia("./resources/sprites/Armor.png", true); // 8
	loadMedia("./resources/sprites/ShortWep.png", true); // 9
	loadMedia("./resources/sprites/MedWep.png", true); // 10
	loadMedia("./resources/sprites/LongWep.png", true); // 11
	loadMedia("./resources/sprites/Shield.png", true); // 12
	loadMedia("./resources/sprites/Scroll.png", true); // 13
	loadMedia("./resources/sprites/Potion.png", true); // 14
	loadMedia("./resources/sprites/Ammo.png", true); // 15
	loadMedia("./resources/sprites/Floor.png", false); // 16
	loadMedia("./resources/sprites/Tile.png", false); // 17
	loadMedia("./resources/sprites/Ground0.png", false); // 18
	loadMedia("./resources/sprites/Fence.png", false); // 19
    loadMedia("./resources/sprites/tilesheet.png", false); // 20
    loadMedia("./resources/HRAFNHEIM.png", false); // 21
    loadMedia("./resources/sprites/Player1.png", true); // 22
    loadMedia("./resources/sprites/Avian1.png", true); // 23
    loadMedia("./resources/sprites/Rodent1.png", true); // 24
    loadMedia("./resources/sprites/Humanoid1.png", true); // 25
    loadMedia("./resources/sprites/Demon1.png", true); // 26
    loadMedia("./resources/sprites/Elemental1.png", true); // 27
    loadMedia("./resources/sprites/Undead1.png", true); // 28
    loadMedia("./resources/sprites/Effect0.png", true); // 29
    loadMedia("./resources/sprites/Effect1.png", true); // 30

    m_fullscreen = 0;

    return true;
}

bool Console::createWindow()
{
    m_root = SDL_CreateWindow(m_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (m_width+m_xBuffer)*m_tileSize, (m_height+m_yBuffer)*m_tileSize, SDL_WINDOW_SHOWN);
    if (m_root == nullptr){
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    } else {
        return true;
    }
}

bool Console::createRenderer()
{
    m_renderer = SDL_CreateRenderer(m_root, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr){
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    } else {
        resetDrawColour();
        return true;
    }
}

bool Console::initImage()
{
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)){
        printf("Failed to initialise SDL_Image! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

bool Console::loadMedia(const char* path, bool setColourKey)
{
    SDL_Surface *loadedSurface = IMG_Load(path);

    if(loadedSurface == nullptr){
        printf("Failed to load image! SDL_image Error: %s\n", IMG_GetError());
        return false;
    } else {
		if (setColourKey){
			uint32_t* pixels = static_cast<uint32_t*>(loadedSurface->pixels); // get pixels from the loaded surface
			SDL_SetColorKey(loadedSurface, SDL_TRUE, pixels[0]); // make the pixel at (0,0) transparent
		}
   	    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, loadedSurface);
	    m_spriteSheets.push_back(texture);
        SDL_FreeSurface(loadedSurface);
        loadedSurface = nullptr;
        return true;
    }
}

void Console::fillBackgroundTile(int x, int y, SDL_Color colour, int scale)
{
	SDL_Rect rect;
	rect.x = x * m_tileSize * scale;
	rect.y = y * m_tileSize * scale;
	rect.w = m_tileSize * scale;
	rect.h = m_tileSize * scale;

	SDL_SetRenderDrawColor(m_renderer, colour.r, colour.g, colour.b, 128);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(m_renderer, &rect);
    resetDrawColour();
}

void Console::fillBackgroundTile(int x, int y, SDL_Color colour, int alpha, int tileSize, int originX, int originY)
{
	SDL_Rect rect;
	rect.x = originX + x * tileSize;
	rect.y = originY + y * tileSize;
	rect.w = tileSize;
	rect.h = tileSize;

	SDL_SetRenderDrawColor(m_renderer, colour.r, colour.g, colour.b, alpha);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(m_renderer, &rect);
    resetDrawColour();
}

void Console::resetDrawColour()
{
    uint8_t r{ m_background.r }, g{ m_background.g }, b{ m_background.b };
    SDL_SetRenderDrawColor(m_renderer, r, g, b, 0x00);
}

void Console::createTiles()
{
	int tileSetDim = 16;
	int numAsciiChars = tileSetDim * tileSetDim;

    for(int i = 0; i < numAsciiChars; i++){
  	    m_glyphs.push_back(Tile(i%tileSetDim, i/tileSetDim, m_tileSize));
    }
}

void Console::render(char* c, int x, int y, SDL_Color colour, int scale, bool scale_pos)
{
    int i = static_cast<int>((*c));

    render(i, x, y, colour, scale, scale_pos);
}

void Console::render(int i, int x, int y, SDL_Color colour, int scale, bool scale_pos)
{
    SDL_Rect dstrect;
    dstrect.x = scale_pos ? x * m_tileSize * scale : x * m_tileSize;
    dstrect.y = scale_pos ? y * m_tileSize * scale : y * m_tileSize;
    dstrect.w = m_tileSize * scale;
    dstrect.h = m_tileSize * scale;

    SDL_Rect srcrect;
    srcrect.w = m_tileSize;
    srcrect.h = m_tileSize;

    srcrect.x = m_glyphs.at(i).m_x;
    srcrect.y = m_glyphs.at(i).m_y;

    SDL_SetTextureColorMod(m_spriteSheets.at(0), colour.r, colour.g, colour.b);
    SDL_RenderCopy(m_renderer, m_spriteSheets.at(0), &srcrect, &dstrect);
}

void Console::renderSprite(int x, int y, int spriteX, int spriteY, int sheet, int scale, bool scale_pos)
{
	SDL_Rect dstrect; // rect to draw on the screen
	dstrect.x = scale_pos ? x * m_tileSize * scale : x * m_tileSize;
	dstrect.y = scale_pos ? y * m_tileSize * scale : y * m_tileSize;
	dstrect.w = m_tileSize * scale;
	dstrect.h = m_tileSize * scale;

	SDL_Rect srcrect; // rect to copy from texture
	srcrect.w = m_tileSize;
	srcrect.h = m_tileSize;

	srcrect.x = spriteX * m_tileSize;
	srcrect.y = spriteY * m_tileSize;

	SDL_RenderCopy(m_renderer, m_spriteSheets.at(sheet), &srcrect, &dstrect);
}

void Console::renderParticle(int x, int y, int spriteX, int spriteY, int spriteSheet, int scale, bool scale_pos)
{
    SDL_Rect dstrect; // rect to draw on the screen
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = m_tileSize * scale;
    dstrect.h = m_tileSize * scale;

    SDL_Rect srcrect; // rect to copy from texture
    srcrect.w = m_tileSize;
    srcrect.h = m_tileSize;

    srcrect.x = spriteX * m_tileSize;
    srcrect.y = spriteY * m_tileSize;

    SDL_RenderCopy(m_renderer, m_spriteSheets.at(spriteSheet), &srcrect, &dstrect);
}

void Console::renderImage(int sheet)
{
    SDL_RenderCopy(m_renderer, m_spriteSheets.at(sheet), NULL, NULL);
}

void Console::update()
{
    SDL_RenderPresent(m_renderer);
}

void Console::flush()
{
    SDL_RenderClear(m_renderer);
}

void Console::setFullscreen()
{
    if (!m_fullscreen){
        m_fullscreen = 1;
        SDL_SetWindowFullscreen(m_root, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else if (m_fullscreen){
        m_fullscreen = 0;
        SDL_SetWindowFullscreen(m_root, 0);
    }
}

void Console::closeSDL()
{
    SDL_Quit();
    IMG_Quit();
}
