#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "graphics/font.h"

class GameController
{
    public:
        GameController(int joystick_id);
        ~GameController();

        void init();
        void simulate(const float t);

        bool button_down(const int i) const;
        float get_axis_value(const int i) const;
        float get_hat_x(const int i) const;
        float get_hat_y(const int i) const;

        void render_gl(); //debug rendering

    protected:
        Font *font; //for debug rendering

        int sdl_joystick_id;
        SDL_Joystick *sdl_joystick;

        int num_axes;
        int num_buttons;
        int num_hats;
        int num_haptics;

        float filter_alpha;
        float *axes;
        float *t_axes;

        bool *buttons;
        Uint8 *hats;
};

#endif // __GAME_CONTROLLER_H__
