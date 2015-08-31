#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include "message_box.h"

using namespace std;

MessageBox::MessageBox() : Widget()
{
    font = NULL;
    time_out_s = 5.0f;

    message_text[0] = '\0';
}

MessageBox::MessageBox(const char *text, float _x, float _y)
{
    strcpy(message_text, text);
    x = _x;
    y = _y;

    font = NULL;
    time_out_s = 5.0f;
}

MessageBox::~MessageBox()
{

}

void MessageBox::init()
{

}

void MessageBox::simulate(const float t)
{
    time_out_s -= t;
}

void MessageBox::render_gl()
{

    glUseProgramObjectARB(0);
    //glDisable(GL_LIGHTING);

    glPushMatrix();
        glLoadIdentity();
        glColor3f(0.0f, 0.0f, 0.0f);
        font->print((int)x, (int)y, message_text);
    glPopMatrix();

}

void MessageBox::set_font(Font *f)
{
    font = f;
}

void MessageBox::set_time_out(const float t)
{
    time_out_s = t;
}

bool MessageBox::timed_out() const
{
    return (time_out_s < 0.0f);
}
