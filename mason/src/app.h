#ifndef __APP_H__
#define __APP_H__

#include <vector>

#include "world.h"
#include "level.h"

#include "widgets/main_window.h"

#define MASON_VERSION_MAJOR 1
#define MASON_VERSION_MINOR 0

#define TEST_LEVEL_FILE "../game/data/levels/test.level"
#define TEST_LEVEL_FILE_WRITE "../game/data/levels/test.level"

class App {
    public:
        App();
        ~App();

        bool init();
        void show_all_ui();

        bool read_from_file(const char *fname);
        bool write_to_file(const char *fname);

        float get_grid_size() const;

        void scroll(const float x, const float y);
        void scroll_reset();
        void get_scroll_state(float &x, float &y);

        void zoom(const float z);
        void zoom_reset();
        void get_zoom_state(float &z);

        void select(float p[2], bool append);
        void select(float p0[2], float p1[2], bool append);

        void move_selection(float p[2]);

        void render_gl();

        Level *get_current_level();
        World *get_world();

        MainWindow *get_main_window();

    private:
        MainWindow *main_window;

        float grid_size;

        float scroll_state[2];
        float zoom_state;

        std::vector<Object2D *> selection;

        World *world;

        Level *current_level;
        char current_level_fname[512];
};

#endif // __APP_H__
