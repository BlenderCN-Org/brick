#include "particle.h"

Particle::Particle()
{
	init();
}

void Particle::init()
{
	pos[0] = pos[1] = pos[2] = 0.0f;
	vel[0] = vel[1] = vel[2] = 0.0f;

	age_left = 2.0f;
	max_age = 2.0f;
}
