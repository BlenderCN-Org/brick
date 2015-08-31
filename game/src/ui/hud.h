#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include "graphics/font.h"
#include "object/player.h"

#define DEFAULT_HUD_FONT_FACE   "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
#define DEFAULT_HUD_FONT_SIZE   10

class HUD
{
    public:
        HUD();
        ~HUD();

        void init();

        void render_gl();
        void simulate(const float t);

        void set_player(Player2D *p);
    private:
        Font *font;
        Player2D *player;
};

#endif // HUD_H_INCLUDED
