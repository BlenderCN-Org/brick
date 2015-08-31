#include <assert.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>


#include "game_controller.h"
#include "ui/hud.h"

using namespace std;

GameController::GameController(int joystick_id)
{
    sdl_joystick_id = joystick_id;
    sdl_joystick = NULL;

    num_axes = 0;
    num_buttons = 0;
    num_hats = 0;
    num_haptics = 0;

    axes = NULL;
    buttons = NULL;
    hats = NULL;

    filter_alpha = 10.0f;

    font = new Font(DEFAULT_HUD_FONT_FACE, DEFAULT_HUD_FONT_SIZE);
}

GameController::~GameController()
{
    SDL_JoystickClose(sdl_joystick);
    if(axes) delete axes;
    if(t_axes) delete t_axes;
    if(buttons) delete buttons;
    if(hats) delete hats;

    delete font;
}

void GameController::init()
{
    //init SDL joystick
    if(SDL_NumJoysticks() > sdl_joystick_id)
    {
        sdl_joystick = SDL_JoystickOpen(sdl_joystick_id);
        assert(sdl_joystick);

        num_axes = SDL_JoystickNumAxes(sdl_joystick);
        num_buttons = SDL_JoystickNumButtons(sdl_joystick);
        num_hats = SDL_JoystickNumHats(sdl_joystick);
        num_haptics = 0;

        axes = new float[num_axes];
        t_axes = new float[num_axes];
        buttons = new bool[num_buttons];
        hats = new Uint8[num_hats];
    }

    font->init();
}

void GameController::simulate(const float t)
{
    //TODO: filter joystick input
    for(int i = 0; i < num_axes; i++)
	{
	    float alpha = 0.2f;//filter_alpha * t;
	    float raw_value = (float)SDL_JoystickGetAxis(sdl_joystick, i) / 32768.0f;
        t_axes[i] = t_axes[i] * (1.0f - alpha) + raw_value * alpha;
        if(fabsf(t_axes[i]) < 0.1f)
        {
            axes[i] = 0.0f;
        }
        else
        {
            axes[i] = (t_axes[i] > 0) ? (t_axes[i] - 0.1f) * 1.3333f : (t_axes[i] + 0.1f) * 1.3333f;
        }


        float len_squared = axes[i] * axes[i];
        if(len_squared > 1.0f)
        {
            float div = 1.0f / sqrtf(len_squared);
            axes[i] = axes[i] * div;
        }
	}

	for(int i = 0; i < num_buttons; i++)
	{
	    buttons[i] = (bool)SDL_JoystickGetButton(sdl_joystick, i);
	}

	for(int i = 0; i < num_hats; i++)
	{
	    hats[i] = SDL_JoystickGetHat(sdl_joystick, i);
	}
}

bool GameController::button_down(const int i) const
{
    assert(i < num_buttons);
    return buttons[i];
}

float GameController::get_axis_value(const int i) const
{
    assert(i < num_axes);
    return axes[i];
}

float GameController::get_hat_x(const int i) const
{
    if(hats[i] & SDL_HAT_LEFT)
    {
        return -1.0f;
    }
    else if(hats[i] & SDL_HAT_RIGHT)
    {
        return 1.0f;
    }
    return 0.0f;
}

void GameController::render_gl()
{
    glUseProgramObjectARB(0);
    //glDisable(GL_LIGHTING);

    glPushMatrix();
        glLoadIdentity();
        glColor3f(0.0f, 0.0f, 0.0f);
        //font->print(0, 460, "seeds: %d", player->get_num_seeds());
        int y_offset = 400;
        int y_height = 16;
        for(int i = 0; i < num_axes; i++)
        {
            font->print(0, y_offset - (i * y_height), "axis %d: %f", i, axes[i]);
        }

        y_offset -= num_axes * y_height;
        for(int i = 0; i < num_buttons; i++)
        {
            font->print(0, y_offset - (i * y_height), "button %d: %d", i, buttons[i]);
        }

        y_offset -= num_buttons * y_height;
        for(int i = 0; i < num_hats; i++)
        {
            font->print(0, y_offset - (i * y_height), "hat %d: %d", i, hats[i]);
        }
    glPopMatrix();
}
