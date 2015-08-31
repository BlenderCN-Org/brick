#ifndef _PLAYER_CONTROLLER_H_
#define _PLAYER_CONTROLLER_H_

#include "object/player.h"

typedef enum {
    INPUT_MOVE_X,
	INPUT_RUN,
	INPUT_JUMP,
	INPUT_JUMP_STOP
} InputMode;

class PlayerController2D
{
public:
	PlayerController2D();
	//PlayerController2D(Player2D *p);

	void process_input(InputMode im, float val = 0.0f);

	void set_player(Player2D *p);
	Player2D *get_player();
private:
	Player2D *player;
	bool is_jumping;  //should maybe live in Player class???
    float run_value;
};



#endif // _PLAYER_CONTROLLER_H_
