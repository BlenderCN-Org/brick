#include <GL/glew.h>
#include <GL/gl.h>

#include "hud.h"

HUD::HUD()
{
    player = NULL;
    font = new Font(DEFAULT_HUD_FONT_FACE, DEFAULT_HUD_FONT_SIZE);
}

HUD::~HUD()
{
    if(font)
    {
        delete font;
    }
}

void HUD::init()
{
    assert(player);
    font->init();
}

void HUD::render_gl()
{
    glUseProgramObjectARB(0);
    //glDisable(GL_LIGHTING);

    glPushMatrix();
        glLoadIdentity();
        glColor3f(0.0f, 0.0f, 0.0f);
        font->print(0, 460, "seeds: %d", player->get_num_seeds());
    glPopMatrix();
}

void HUD::simulate(const float t)
{

}

void HUD::set_player(Player2D *p)
{
    player = p;
}
