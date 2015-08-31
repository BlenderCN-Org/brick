#ifndef _LEVEL_PLATFORM_H_
#define _LEVEL_PLATFORM_H_

#include <fstream>

#include "object.h"

using namespace std;

//basically something that you can walk on
//it could move, it could be static, it could be a rectangle,
//it could be a more complex shape... but the player collides
//with it
class LevelPlatform2D : public Object2D
{
	friend class Level;
	friend class World;
	public:
		LevelPlatform2D();
		LevelPlatform2D(const float *_pos, const float *_dim);

		void render_gl();

		void init();
		void simulate(const float t);

		bool read_from_file(const char *fname);

#ifdef __EDITOR__
		bool write_to_file(ofstream &f);
#endif //__EDITOR__

		void set_angle(const float t);
		void set_angular_velocity(const float v);

		bool hit_test_2d(float p[2]);
	private:
		//float vel[2];

		float theta;
		float angular_velocity;
		bool rotates;

		float track_position;
		float track_rate;
};

#endif // _LEVEL_PLATFORM_H_
