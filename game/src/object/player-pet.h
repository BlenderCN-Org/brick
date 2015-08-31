#ifndef _PLAYER_PET_H_
#define _PLAYER_PET_H_

#include "level.h"
#include "object.h"

class World;

class PlayerPet : public Object2D
{
	friend class World;
	public:
		PlayerPet();

		void init();
		void simulate(const float t);

		void render_gl();
	private:
		float vel[2];
};

#endif // _PLAYER_PET_H_
