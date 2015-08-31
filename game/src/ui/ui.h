#ifndef __UI_H__
#define __UI_H__

#include <vector>

#include "graphics/font.h"
#include "message_box.h"

class UI
{
    public:
        UI();
        ~UI();

        void init();
        void simulate(const float t);
        void render_gl();

        void message_box(const char *text, float x, float y, float time_out, bool modal = false);
    private:
        Font *font;

        std::vector<MessageBox *> active_mb;
};

#endif // __UI_H__
