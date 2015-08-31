#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "math/math-utility.h"

class Particle {
	friend class ParticleManager;
	friend class ParticleSystem;
	public:
		Particle();

		inline void simulate(const float t);
		inline void init();

	private:
		inline bool is_alive() { return age_left > 0.0f; }

		Math::Float3 pos;
		Math::Float3 vel;
		float age_left;		//how much longer the particle will live
        float max_age;      //maximum age
};

void Particle::simulate(const float t)
{
	age_left -= t;	//age the particle

	float gravity = -0.1f; //placeholder - implement real gravity
	float drag = 0.03f;		//placeholder - implement real drag
	for(int i = 0; i < 3; i++)
	{
		//vel[i] = vel[i] - gravity * t;
		pos[i] = pos[i] + vel[i] * t;
	}

	//gravity
	vel[1] += gravity;

	//drag
	vel[0] -= vel[0] * drag;
	vel[1] -= vel[1] * drag;
	vel[2] -= vel[2] * drag;
}

#endif // __PARTICLE_H__
