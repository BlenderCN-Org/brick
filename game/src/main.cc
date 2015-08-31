#include <iostream>
#include <exception>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <libxml/parser.h>

#include <Box2D/Box2D.h>
#include <SDL2/SDL.h>

#include <math.h>


#include "world.h"
#include "player-controller.h"
#include "math/perlin.h"
#include "math/math-utility.h"
#include "graphics/render_shepherd.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/font.h"
#include "audio/sdl-audio.h"
#include "debug/console.h"
#include "fluid2-d.h"
#include "ui/ui.h"
#include "ui/hud.h"
#include "input/game_controller.h"

using namespace std;
using namespace Math;
using namespace SDLAudio;

// Width & Height of window
#define WIN_WIDTH  640
#define WIN_HEIGHT 480

#define SCROLL_SPEED 0.02f

//#define DEFAULT_FONT    "/usr/share/cups/fonts/FreeMono.ttf"
//#define DEFAULT_FONT    "/usr/share/cups/fonts/FreeMonoOblique.ttf"
#define DEFAULT_FONT    "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
//#define DEFAULT_FONT    "/usr/share/fonts/truetype/kacst/KacstBook.ttf"

bool use_render_shepherd = false;

float scroll_offset[2] =	{ 0.0f, 0.0f };
float ground_color[4] =		{ 0.70, 0.75, 0.78, 1.0f };
float space_color[4] =		{ 0.0f, 0.05f, 0.15f, 1.0f };

RenderShepherd *rs = NULL;

HUD *hud = NULL;
World *world = NULL;
PlayerController2D pc;
DebugConsole *console = NULL;
UI *ui = NULL;

//SDL_Joystick *sdl_joystick = NULL;
bool game_controller_debug_render = false;
GameController *game_controller = NULL;

bool console_active = false;

bool Game_paused = false;
bool Game_pause_key_down_last_frame = false;

Texture *tex_test;
Font *test_font;

SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_gl_context = NULL;

// Kill program
void endProgram(int code)
{
    SDL_Quit();
    exit(code);
}

// Process SDL events
void processEvents()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if(keystate[SDLK_ESCAPE]) endProgram(0);

    //THIS CODE IS ALL SORTS OF FUCKED UP
    if(game_controller)
    {

        bool start_pressed = false;
        if(game_controller->button_down(7))
        {
            //start_pressed = true;
            Game_pause_key_down_last_frame = true;
        }
        else if(Game_pause_key_down_last_frame)
        {
            //pause game
            start_pressed = true;
            //Game_pause_timer = 0.25f;
            cout<<"paused: "<<Game_paused<<endl;
            Game_pause_key_down_last_frame = false;
        }
        if(start_pressed)
        {
            Game_paused = ! Game_paused;
        }

        float run_pct = Math::clamp(0.5f * game_controller->get_axis_value(2) + 0.5f, 0.0f, 1.0f);
        pc.process_input(INPUT_RUN, run_pct);

        float move_x = (game_controller->get_axis_value(0) + game_controller->get_hat_x(0));
        pc.process_input(INPUT_MOVE_X, move_x);


        /*move_x = run_mod * game_controller->get_axis_value(5);
        pc.process_input(INPUT_MOVE_X, move_x);*/

        if(game_controller->button_down(0))
        {
            pc.process_input(INPUT_JUMP);
        } else {
            pc.process_input(INPUT_JUMP_STOP);
        }

        if(game_controller->button_down(1))
        {
            world->blast();
        }

        if(game_controller->button_down(5))
        {
            if(world->get_player()->plant_seed())
            {
                world->drop_seed(world->get_player()->get_pos());
            }
        }
    }

	if(!console_active)
	{
        if(keystate['a']) pc.process_input(INPUT_MOVE_X, -1.0f);
        if(keystate['d']) pc.process_input(INPUT_MOVE_X, 1.0f);
        if(keystate['q']) { world->clear_crates(); }
        if(keystate['k']) { world->blast(); }
        if(keystate['l']) { world->attract(); }
        if(keystate['p']) { if(world->get_player()->plant_seed()) { world->drop_seed(world->get_player()->get_pos()); } }
        if(keystate['o']) { world->shit_a_brick(); }
        if(keystate['w']) {
            pc.process_input(INPUT_JUMP);
            //world->shit_a_brick();
        }
        else
        {
            pc.process_input(INPUT_JUMP_STOP);
        }
	}


	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			endProgram(0);
		}
		if(event.type == SDL_KEYDOWN)
		{
            //send all keystrokes to the console
			if(console_active && event.key.keysym.sym != '`')
			{
			    Uint8 character = event.key.keysym.sym;
			    //cout<<character<<endl;
			    bool send_char = true;
                if(character == 47 && keystate[SDLK_RSHIFT])
                {
                    send_char = keystate['/'];
                }
                if(character == 48 && keystate[SDLK_LSHIFT])
                {
                    send_char = keystate['0'];
                }
                if(character == '-' && (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]))
                {
                    send_char = false;
                    console->receive_char('_');
                }
                if(send_char)
                {
                    if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT])
                    {
                        character += 'A' - 'a';
                    }
                    console->receive_char(character);
                }
			}
			switch(event.key.keysym.sym)
			{
				case SDLK_UP:
					//SDLAudio::play_sound(AUDIO_TEST_FILE);
					break;
                case SDLK_DOWN:
                    break;
                case '`':
                    console_active = !console_active;
                    console->activate(console_active);
                    if(console_active)
                    {
                        Game_paused = true;
                    }
                    else
                    {
                        Game_paused = false;
                    }
                    break;
                default:
                    break;
			}
		}
	}
}

void mainLoop() {
	float last_time = 0.0f;
	Player2D *player = world->get_player();

    while(true) {
		Uint32 ticks = SDL_GetTicks();
		float game_time = (float)ticks;
		float frame_time = (game_time - last_time) / 1000.0f;

		//update the game world
		//Game_pause_timer -= frame_time;
		if(!Game_paused)
		{
		    world->simulate(frame_time, game_time);
		}

		hud->simulate(frame_time);
        console->simulate(frame_time);
        ui->simulate(frame_time);
        if(game_controller)
        {
            game_controller->simulate(frame_time);
        }

		float player_x, player_y;
		player->get_pos(player_x, player_y);

		scroll_offset[0] += SCROLL_SPEED * (player_x - scroll_offset[0]);
		scroll_offset[1] += SCROLL_SPEED * (player_y - scroll_offset[1]);
		scroll_offset[1] = max(scroll_offset[1], -1.0f);

		float eye[3] = { 0.0f, 0.0f, 0.0f };

		eye[0] = scroll_offset[0];
		eye[1] = scroll_offset[1] + 0.7f * scroll_offset[1] + 10.0f;

		float look_at_pos[3] = {
			player_x,//eye[0],
			player_y,//eye[1] / 5.0f,
			-20.0f
		};

		processEvents();

        if(use_render_shepherd)
        {
            //glFlush();
            //SDL_GL_SwapBuffers(); // Update screen
            assert(rs);
            rs->eye[0] = scroll_offset[0];
            rs->eye[1] = scroll_offset[1] + 0.7f * scroll_offset[1] + 10.0f;
            rs->eye[2] = 0.0f;

            rs->look_at_pos[0] = player_x;
            rs->look_at_pos[1] = player_y;
            rs->look_at_pos[2] = -20.0f;

            rs->render();
        }
        else
        {
            //scrolling
            glLoadIdentity(); //Reset orientation
            //glTranslatef(eye[0], eye[1], 0.0f);
            gluLookAt(eye[0], eye[1], eye[2],
                      look_at_pos[0], look_at_pos[1], look_at_pos[2],
                      0.0f, 1.0f, 0.0f);



            float height_blend = clamp((player_y) / 30.0f, 0.0f, 1.0f);
            float clear_color[4];
            clear_color[0] = lerp(ground_color[0], space_color[0], height_blend);
            clear_color[1] = lerp(ground_color[1], space_color[1], height_blend);
            clear_color[2] = lerp(ground_color[2], space_color[2], height_blend);
            clear_color[3] = 1.0f;

            //glClearColor(0.75f, 0.75, 0.78, 1.0f);
            glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear color and depth buffer

            //set fog values
            glFogfv(GL_FOG_COLOR, clear_color);
            //glFogf(GL_FOG_DENSITY, 0.5f);
            glFogf(GL_FOG_START, 5.0f);
            glFogf(GL_FOG_END, 50.0f);

            //add a light on the player
            float x, y;
            world->get_player()->get_pos(x, y);
            GLfloat light_pos[4] = { x, y, 2.0f, 1.0f };
            GLfloat light_color[4] = { 8.0f, 5.0f, 20.0f, 0.0f };
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
            glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
            //glLightfv(GL_LIGHT0, GL_COLOR, light_color);
            glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5f);
            glEnable(GL_LIGHT0);

            //set up a top-down light
            GLfloat top_light_pos[4] = { 0.0f, 500.0f, 20.0f, 1.0f };
            GLfloat top_light_col[4] = { 10.0f, 50.0f, 100.0f, 1.0f };
            glLightfv(GL_LIGHT1, GL_DIFFUSE, top_light_col);
            glLightfv(GL_LIGHT1, GL_POSITION, top_light_pos);
            //glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
            glEnable(GL_LIGHT1);

            glTranslatef(0.0f, 0.0f, -22.0f);

            world->render_gl();

            //fluid_test->render_gl();
        }

        hud->render_gl();
        if(console->is_active())
        {
            console->render_gl();
        }

        ui->render_gl();

        if(game_controller_debug_render)
        {
            game_controller->render_gl();
        }
        glFlush();
        //SDL_GL_SwapBuffers(); // Update screen
        SDL_GL_SwapWindow(sdl_window);
		last_time = game_time;
    }
}

// Setup OpenGL perspective
void init_gl() {

	GLenum res = glewInit();
	assert(res == GLEW_OK);

	cout<<"OpenGL version: "<<glGetString(GL_VERSION)<<endl;

	char *extensions = (char *)malloc(strlen((char *)glGetString(GL_EXTENSIONS))+1);
	strcpy(extensions, (char *)glGetString(GL_EXTENSIONS));
	int len = strlen(extensions);
	for(int i = 0; i < len; i++) {
		if(extensions[i] == ' ') {
			extensions[i] = '\n';
		}
	}
	cout<<extensions<<endl;
	free(extensions);

	/*
	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for(i = 0; i < n; i++) {
		cout<<glGetStringi(GL_EXTENSIONS, i);
	}*/

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glHint(GL_FOG_HINT, GL_DONT_CARE);

	gluPerspective(32, (float)WIN_WIDTH / WIN_HEIGHT, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	cout<<"got here"<<endl;
}

// Init everything
int main(int argc, char* argv[]) {
    rs = new RenderShepherd;

	console = new DebugConsole;

	//create the game world
	world = new World;
	hud = new HUD;
	ui = new UI;

	world->set_render_shepherd(rs);

	Player2D *player = world->get_player();

	//attach the player to the player controller
	pc.set_player(player);

	LIBXML_TEST_VERSION

	sdl_window = SDL_CreateWindow("Shit a Brick!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(sdl_window);

    //SDL_Init(SDL_INIT_EVERYTHING);//SDL_INIT_VIDEO);
    //SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);
    //SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, 24, /*SDL_FULLSCREEN | */SDL_OPENGL | SDL_GL_DOUBLEBUFFER);

    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    assert(sdl_gl_context);

    SDL_GL_SetSwapInterval(1);

	//SDL_EnableKeyRepeat(1, SDL_DEFAULT_REPEAT_INTERVAL);
    //this is better for the console / general typing
    //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    //SDL_EnableUNICODE(true);

    int num_joysticks = SDL_NumJoysticks();
    if(num_joysticks > 0)
    {
        game_controller = new GameController(0);
        assert(game_controller);
        game_controller->init();
    }

    if(use_render_shepherd)
    {
        rs->init();
    }
    else
    {
        init_gl();
    }

	glutInit(&argc, argv);
	SDLAudio::init();

    console->set_world(world);
    console->init();
    console->register_switch(&game_controller_debug_render, "render_joystick");
    console->register_switch(&use_render_shepherd, "render_shepherd");

	world->init();

	hud->set_player(player);
	hud->init();

	ui->init();
	ui->message_box("Plant a seed.", 250.0f, 450.0f, 10.0f);

    mainLoop();



	delete world;
	delete hud;
	if(game_controller)
	{
        delete game_controller;
	}

	delete rs;

	SDL_GL_DeleteContext(sdl_gl_context);
    SDL_DestroyWindow(sdl_window);
	SDL_Quit();

    return 0;
}
