#ifndef CONSOLE_H
#define CONSOLE_H

#include "SDL.h"
#include <vector>

#include "Tile.h"

class Console
{
    public:
        Console(int width, int height, const char* title, const char* path, int tileSize);
        virtual ~Console();

        int Getm_width() { return m_width; }
        void Setm_width(int val) { m_width = val; }
        int Getm_height() { return m_height; }
        void Setm_height(int val) { m_height = val; }
		int getTileSize() { return m_tileSize; } 
        int getXBuffer(){return m_xBuffer;}
        int getYBuffer(){return m_yBuffer;}
		void toggleAsciiMode(){ displayAscii = !displayAscii; }
		bool getDisplayAscii(){ return displayAscii; }

        bool init(const char* path);
        bool createWindow();
        bool createRenderer();
        bool initImage();
        bool loadMedia(const char* path, bool setColourKey);
        void createTiles();
        void render(char* c, int x, int y, SDL_Color colour, int scale = 1, bool scale_pos = true);
        void render(int i, int x, int y, SDL_Color colour, int scale = 1, bool scale_pos = true);
        void update();
        void closeSDL();
        void flush();
        void setFullscreen();
        SDL_Renderer* getRenderer(){return m_renderer;};
		void fillBackgroundTile(int x, int y, SDL_Color colour, int scale = 1);
		void fillBackgroundTile(int x, int y, SDL_Color colour, int alpha, int tileSize, int originX, int originY);

		void renderSprite(int x, int y, int spriteX, int spriteY, int spriteSheet, int scale = 1, bool scale_pos = true);
        void renderImage(int spriteSheet);
        void renderParticle(int x, int y, int spriteX, int spriteY, int spriteSheet, int scale, bool scale_pos = true);

        void resetDrawColour();
        const SDL_Color getDrawColour() const { return m_background; };

    protected:

    private:
        SDL_Window *m_root;
        SDL_Renderer *m_renderer;
        int m_width;
        int m_height;
        const char *m_title;
        int m_fullscreen;
        int m_tileSize;
        int m_xBuffer;
        int m_yBuffer;
        std::vector<Tile> m_glyphs;
		bool displayAscii;
		std::vector<SDL_Texture*> m_spriteSheets;
        SDL_Color m_background;
};

#endif // CONSOLE_H
