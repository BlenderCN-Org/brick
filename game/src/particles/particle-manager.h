#ifndef _PARTICLE_MANAGER_H
#define _PARTICLE_MANAGER_H

#include <vector>

#include "graphics/texture.h"

#include "particle.h"

#define MAX_PARTICLES			1000
#define MAX_PARTICLE_SYSTEMS	100


class ParticleSystem;
//
// manager class for all particle systems
// also contains an allocation pool for new particles
//
class ParticleManager {
	friend class ParticleSystem;
	public:
		ParticleManager();
		~ParticleManager();

        bool init();
		void simulate(const float t);
		void render_gl();

        ParticleSystem *allocate_particle_system();
        void run_particle_system(ParticleSystem *p, float pos[3]);

	private:
		std::vector<ParticleSystem *> active_systems;
		std::vector<ParticleSystem *> system_pool;

		std::vector<Particle *> particle_pool;
};

#endif
