#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "player-controller.h"

#define MAX_GROUND_VEL              5.0f
#define MAX_GROUND_VEL_RUNNING      15.0f
#define GROUND_ACCEL_RUNNING        3.0f
#define GROUND_ACCEL                1.5f

using namespace std;

PlayerController2D::PlayerController2D()
{
	player = NULL;

	run_value = 0.0f;
}

void PlayerController2D::process_input(InputMode im, float val)
{
	switch(im)
	{
	    case INPUT_MOVE_X:
	    {
	        b2Vec2 v = player->b2d_body->GetLinearVelocity();

	        float ground_acceleration = Math::lerp(GROUND_ACCEL, GROUND_ACCEL_RUNNING, run_value);

            v.x += ground_acceleration * val;
            float speed_limit = Math::lerp(MAX_GROUND_VEL, MAX_GROUND_VEL_RUNNING, run_value);
            v.x = Math::clamp(v.x, -speed_limit, speed_limit);
            player->b2d_body->SetLinearVelocity(v);

	        break;
	    }
		case INPUT_JUMP:
			if(!is_jumping) {// && player->ground != NULL) {
				b2Vec2 v = player->b2d_body->GetLinearVelocity();
				v.y = 12.5f;
				player->b2d_body->SetLinearVelocity(v);
			}
			is_jumping = true;
			break;
		case INPUT_JUMP_STOP:
			is_jumping = false;
			break;
		case INPUT_RUN:
            run_value = val;
			break;
	}
}

void PlayerController2D::set_player(Player2D *p)
{
	player = p;
	is_jumping = false;
}

Player2D *PlayerController2D::get_player()
{
	return player;
}

