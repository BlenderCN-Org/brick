#include <iostream>

#include "ui.h"
#include "message_box.h"

using namespace std;

#define UI_DEFAULT_FONT_FACE    "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
#define UI_DEFAULT_FONT_SIZE    12

UI::UI()
{
    font = NULL;
}

UI::~UI()
{
    if(font)
    {
        delete font;
    }

    std::vector<MessageBox *>::iterator mbi;
    std::vector<MessageBox *>::iterator first, last;
    first = active_mb.begin();
    last = active_mb.end();
    for(mbi = first; mbi != last; mbi++)
    {
        MessageBox *mb = *mbi;
        delete mb;
    }
}

void UI::init()
{
    font = new Font(UI_DEFAULT_FONT_FACE, UI_DEFAULT_FONT_SIZE);
    font->init();
}

void UI::simulate(const float t)
{
    std::vector<std::vector<MessageBox *>::iterator> delete_list;

    std::vector<MessageBox *>::iterator mbi, first, last;
    first = active_mb.begin();
    last = active_mb.end();
    for(mbi = first; mbi != last; mbi++)
    {
        MessageBox *mb = *mbi;
        mb->simulate(t);
        if(mb->timed_out())
        {
            delete_list.push_back(mbi);
        }
    }

    //delete all the message boxes that have timed out
    std::vector<std::vector<MessageBox *>::iterator>::iterator mbii;
    for(mbii = delete_list.begin(); mbii != delete_list.end(); mbii++)
    {
        mbi = *mbii;
        MessageBox *mb = *mbi;
        active_mb.erase(mbi);
        delete mb;
    }
}

void UI::render_gl()
{
    std::vector<MessageBox *>::iterator mbi, first, last;
    first = active_mb.begin();
    last = active_mb.end();
    for(mbi = first; mbi != last; mbi++)
    {
        MessageBox *mb = *mbi;
        mb->render_gl();
    }
}

void UI::message_box(const char *text, float x, float y, float time_out, bool modal)
{
    MessageBox *mb = new MessageBox(text, x, y);

    mb->set_font(font);
    mb->init();
    mb->set_time_out(time_out);

    active_mb.push_back(mb);

    cout<<text<<endl;
}
