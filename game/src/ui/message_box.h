#ifndef _MESSAGE_BOX_H_
#define _MESSAGE_BOX_H_

#include "widget.h"
#include "graphics/font.h"

class MessageBox : public Widget
{
    public:
        MessageBox();
        MessageBox(const char *text, float _x, float _y);
        ~MessageBox();

        virtual void init();
        virtual void simulate(const float t);
        virtual void render_gl();

        void set_font(Font *f);
        void set_time_out(const float t);

        bool timed_out() const;
    private:
        Font *font;

        char message_text[512];

        float time_out_s;
};

#endif // _MESSAGE_BOX_H_
