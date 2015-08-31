#ifndef _PLAYER_H_
#define _PLAYER_H_

#define PLAYER_WIDTH 0.3f
#define PLAYER_HEIGHT 0.4f

#include "object.h"
#include "graphics/shader.h"

class World;
class PlayerController2D;
class LevelPlatform2D;

class Player2D : public Object2D
{
friend class PlayerController2D;
//friend class CameraController2D;
friend class World;
public:
	Player2D();
	~Player2D();

	void init();
	void simulate(const float t);
	void render_gl();

	bool plant_seed();

	int get_num_seeds();
	void set_num_seeds(int s);

private:
	LevelPlatform2D *ground;

	int num_seeds;
	float seed_cooldown_timer;

	//temp
	Shader shader;
};

#endif // _PLAYER_H_
