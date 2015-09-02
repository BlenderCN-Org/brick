#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <vector>
#include <string>

#include <GL/glew.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif //__APPLE__



#include "graphics/font.h"
#include "world.h"

#define DEFAULT_CONSOLE_FONT_FACE   "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
//#define DEFAULT_CONSOLE_FONT_FACE "data/fonts/sin_city.ttf"
#define DEFAULT_CONSOLE_FONT_SIZE   10

using namespace std;

class DebugConsole
{
    public:
        DebugConsole();
        ~DebugConsole();

        void init();

        void activate(bool a);
        bool is_active() const;

        void receive_char(const char c);
        void execute();

        void simulate(const float dt);

        void render_gl();

        void set_world(World *w);
        void register_switch(bool *b, const char *name);
    private:
        void traverse_command_history(char c);
        float pct_exposed;
        bool active;

        Font *font;
        string current_command;

        int command_history_idx;
        std::vector<string> command_history;

        std::vector<string> boolean_switch_names;
        std::vector<bool *> boolean_switches;

        World *world;
};

#endif // __CONSOLE_H__
