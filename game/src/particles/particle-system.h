#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include <vector>

//#include <GL/glew.h>
//#include <GL/gl.h>
#include <math/math-utility.h>

#include "graphics/texture.h"
#include "particle.h"
#include "particle-manager.h"

enum EmitterShape {
	EMITTER_SHAPE_SPHERE,
	EMITTER_SHAPE_POINT,
	EMITTER_SHAPE_LINE,
	EMITTER_SHAPE_SQUARE,
	EMITTER_SHAPE_CUBE,

	NUM_EMITTER_SHAPES
};

#define PSYS_LOOPING                (1<<0)
#define PSYS_KILL_PARTICLES         (1<<1)

typedef struct {
    int flags;
    float duration;
    float emission_rate;
    int initial_particle_count;
    float particle_lifespan[2];

    float tint_start[3];
    float tint_end[3];

    float scale_start[2];
    float scale_end[2];

    Texture *texture;
} ParticleSystemTemplate;

//class ParticleManager;
class ParticleSystem {
	friend class ParticleManager;
	public:
		ParticleSystem();
		~ParticleSystem();

		void init();
		void init(const ParticleSystemTemplate *pst);

		void simulate(const float frame_time);

		void init_gl();
		void render_gl();

	private:
		inline void spawn_new(const float frame_time, ParticleManager *pm, int spawn_count);

		EmitterShape emitter_shape;

		float max_age;                  //should be in sequence?
		float age_left;                 //how many seconds left
		int flags;                      //particle system flags

		float emission_rate;			//particles / second
		int initial_particle_count;     //initial particle count
		float time_since_spawn;			//time a particle was last spawned
		float lifespan[2];				//particle lifespan range (seconds);

		float tint_start[3];
		float tint_end[3];

		float scale_start[2];
		float scale_end[2];

		float system_pos[3];

		std::vector<Particle *> particle_list;

		GLuint gl_vbo;
		GLuint gl_ibo;

		Texture *tex;
};

//maybe shouldn't be inline?
void ParticleSystem::spawn_new(const float frame_time, ParticleManager *pm, int spawn_count = 0)
{
	//figure out if we actually want to spawn a new particle now

	/*
	 * let's say emission rate is 10 particles/s...
	 * that means a particle should spawn every 1/10th of a second...
	 * so, frame_time_s * 10
	 */

	int num_particles = (int)(emission_rate * time_since_spawn);
	if(num_particles > 0) {
		time_since_spawn = 0.0f;
	} else {
		time_since_spawn += frame_time;
	}

	num_particles += spawn_count;

	//ok, now actually spawn the particles
	for(int i = 0; i < num_particles; i++) {
	    if(pm->particle_pool.size() > 0)
	    {
            Particle *new_p = *(pm->particle_pool.begin());
            assert(new_p);

            new_p->init();

            new_p->age_left = Math::random(lifespan[0], lifespan[1]);
            new_p->max_age = new_p->age_left;

            new_p->pos[0] = system_pos[0];
            new_p->pos[1] = system_pos[1];
            new_p->pos[2] = system_pos[2];

            new_p->vel[0] = Math::random(-50.0f, 50.0f);
            new_p->vel[1] = Math::random(-50.0f, 50.0f);
            new_p->vel[2] = Math::random(-50.0f, 50.0f);
            float length = sqrt(new_p->vel[0] * new_p->vel[0] + new_p->vel[1] * new_p->vel[1] + new_p->vel[2] * new_p->vel[2]);
            new_p->vel[0] = 20.0f * new_p->vel[0] / length;
            new_p->vel[1] = 20.0f * new_p->vel[1] / length;
            new_p->vel[2] = 20.0f * new_p->vel[2] / length;

            pm->particle_pool.erase(pm->particle_pool.begin());
            particle_list.push_back(new_p);
	    }
	}
}

#endif // __PARTICLE_SYSTEM_H__
