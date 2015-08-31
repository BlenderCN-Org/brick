#include <iostream>

#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <assert.h>
#include <math.h>

#include "math/math-utility.h"
#include "particle-manager.h"
#include "particle-system.h"

#define GL_RENDER_BATCH_SIZE	100


//

ParticleManager::ParticleManager()
{
	//allocate all the particles
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		Particle *p = new Particle;
		assert(p);

		particle_pool.push_back(p);
	}

    //allocate all the systems
    for(int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
    {
        ParticleSystem *ps = new ParticleSystem;
        assert(ps);

        system_pool.push_back(ps);
    }
}

ParticleManager::~ParticleManager()
{
	//deallocate all the particles
	std::vector<Particle *>::iterator it;
	for(it = particle_pool.begin(); it != particle_pool.end(); it++) {
		Particle *p = *it;
		delete p;
	}

	//remove all the references from the vector
	while(particle_pool.size() > 0) {
		particle_pool.erase(particle_pool.begin());
	}


	std::vector<ParticleSystem *>::iterator psi;
	for(psi = system_pool.begin(); psi != system_pool.end(); psi++)
	{
	    ParticleSystem *ps = *psi;
	    delete ps;
	}

	while(system_pool.size() > 0)
	{
	    system_pool.erase(system_pool.begin());
	}
}

bool ParticleManager::init()
{
	//add a particle system for shits and / or giggles
	/*ParticleSystem *ps = new ParticleSystem;
	ps->init();
	ps->emission_rate = 400.0f;
	active_systems.push_back(ps);*/

	return true;
}

void ParticleManager::simulate(const float t)
{
	//std::cout<<"ParticleManager::simulate()"<<std::endl;
	//for(int i = 0; i < num_active_systems; i++) {
	std::vector<ParticleSystem *>::iterator ps_i = active_systems.begin();
	while(ps_i != active_systems.end()) {
		ParticleSystem *ps = *(ps_i);
		assert(ps);

		ps->age_left -= t;

		//maybe spawn new particles
		bool system_active = true;
		if(ps->age_left > 0.0f)
		{
            ps->spawn_new(t, this);
		} else {
            if(ps->flags & PSYS_LOOPING)
            {
                ps->age_left = ps->max_age;
            }
		    if(ps->flags & PSYS_KILL_PARTICLES)
		    {
		        //need to kill all the particles and send them
		        //back to the particle pool!
		        assert(false);
		        system_active = false;
		    }
		    if(ps->particle_list.size() == 0)
		    {
		        system_active = false;
            }
		}

		std::vector<Particle *>::iterator it = ps->particle_list.begin();
		while(it != ps->particle_list.end()) {
			Particle *p = *it;
			assert(p);

			p->simulate(t);

			if(p->is_alive()) {
				it++;	//next particle
			} else {
				//back to whence ye came! (the particle pool)
				ps->particle_list.erase(it);
				particle_pool.push_back(p);
			}
		}

		//if the particle system is still active / has the
		//potential to keep emitting, keep it around, and move
		//on to the next system. Otherwise, we don't need to
		//process it anymore, so get rid of it.
        if(system_active)
		{
		   ps_i++;
		} else {
		    ParticleSystem *ps = *ps_i;
		    active_systems.erase(ps_i);
		    system_pool.push_back(ps);
		}
	}
}

void ParticleManager::render_gl()
{
	for(int i = 0; i < active_systems.size(); i++) {
		active_systems[i]->render_gl();
	}
}

ParticleSystem *ParticleManager::allocate_particle_system()
{
    if(system_pool.size() > 0)
    {
        ParticleSystem *ps = *(system_pool.begin());
        assert(ps);

        system_pool.erase(system_pool.begin());
        return ps;
    } else {
        return NULL;
    }
}

void ParticleManager::run_particle_system(ParticleSystem *ps, float pos[3])
{
    ps->system_pos[0] = pos[0];
    ps->system_pos[1] = pos[1];
    ps->system_pos[2] = pos[2];

    ps->spawn_new(0.0f, this, ps->initial_particle_count);

    active_systems.push_back(ps);
}
