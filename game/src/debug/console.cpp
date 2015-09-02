#include "console.h"

#include <iostream>
#include <string.h>

#include "math/math-utility.h"

#define EXPOSE_PERCENT_RATE 3.0f

DebugConsole::DebugConsole()
{
    font = NULL;
    pct_exposed = 0.0f;
    active = false;

    command_history_idx = 0;
}

DebugConsole::~DebugConsole()
{
    delete font;
}

void DebugConsole::activate(bool a)
{
    active = a;
}

bool DebugConsole::is_active() const
{
    if(active)
    {
        return true;
    }
    else if(pct_exposed < 0.01f)
    {
        return false;
    }
    return true;
}

void DebugConsole::init()
{
    //the font allocation has to happen here (otherwise it
    //will be initialized before OpenGL and won't work. The
    //real solution here is to change the font code so that the
    //init work isn't done in the constructor.
    font = new Font(DEFAULT_CONSOLE_FONT_FACE, DEFAULT_CONSOLE_FONT_SIZE);
    font->init(); //currently does nothing

    current_command.clear();
    command_history.clear();
}

void DebugConsole::simulate(const float dt)
{
    if(active)
    {
        pct_exposed += dt * EXPOSE_PERCENT_RATE;
    }
    else
    {
        pct_exposed -= dt * EXPOSE_PERCENT_RATE;
    }
    pct_exposed = Math::clamp(pct_exposed, 0.0f, 1.0f);
}

void DebugConsole::receive_char(const char c)
{
    cout<<(int)c<<endl;
    switch(c)
    {
        case 13:    //RETURN
            execute();
            break;
        case 8:     //BACKSPACE
            if(current_command.size() > 0)
            {
                current_command.erase(current_command.end() - 1, current_command.end());
            }
            break;
        case 17:    //UP ARROW
        case 18:    //DOWN ARROW
            traverse_command_history(c);
            break;
        default:
            current_command.push_back(c);
            break;
    }
}

void DebugConsole::execute()
{
    for(int i = 0; i < boolean_switch_names.size(); i++)
    {
        if(current_command == boolean_switch_names[i])
        {
            bool *b = boolean_switches[i];
            *b = !(*b);
        }
    }

    /*if(!current_command.compare("drop_seed"))
    {
        Float2 drop_pos = world->get_player()->get_pos();
        world->drop_seed(drop_pos);
    }*/
    command_history.push_back(current_command);
    command_history_idx = command_history.size() - 1;
    current_command.clear();
}

void DebugConsole::render_gl()
{
    //animation
    float v_offset = Math::cerp(-0.5f, 0.0f, pct_exposed);

    glUseProgramObjectARB(0);

    glPushAttrib(GL_DEPTH_BUFFER_BIT);

    //first render the backdrop
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0.0f, v_offset, 0.0f);

    glColor4f(0.6f, 0.4f, 1.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.4f);              // Top Left
        glVertex2f( 1.0f, -0.4f);              // Top Right
        glVertex2f( 1.0f, -1.0f);              // Bottom Right
        glVertex2f(-1.0f, -1.0f);              // Bottom Left
    glEnd();
    glPopMatrix();


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);


    //now render the text
    glColor3f(0.2f, 0.0f, 0.0f);

    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0.0f, v_offset * 256, 0.0f);

    char command_line[512];
    sprintf(command_line, "your wish > %s", current_command.c_str());
    font->print(0, 120, command_line);
    glPopMatrix();

    glPopAttrib();
}

void DebugConsole::set_world(World *w)
{
    assert(w);
    world = w;
}

void DebugConsole::register_switch(bool *b, const char *name)
{
    std::string n(name);
    boolean_switch_names.push_back(n);
    boolean_switches.push_back(b);
}

void DebugConsole::traverse_command_history(char c)
{
    if(command_history.size() > 0)
    {
        current_command = command_history[command_history_idx];
        command_history_idx = (c == 17) ? command_history_idx - 1 : command_history_idx + 1;
        if(command_history_idx < 0)
        {
            command_history_idx = 0;
        }
        if(command_history_idx >= command_history.size())
        {
            command_history_idx = command_history.size() - 1;
        }
    }
}
