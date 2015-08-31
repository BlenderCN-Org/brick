//lessons from proteus:
/*
 * When the graphics are simple, you just don't expect much,
 * so when cool things happen, they're much more rewarding.
 *
 * natural phenomena are captured in very simple ways
 * wildlife is really rewarding - fun to stay back and watch
 * them interact with the environment. The fact that they're
 * scared and run away is actually pretty cool because of that.
 *
 * playing with animals
 *
 * the lack of direction makes it very easy going; no stress
 * ever that you're in the wrong place or doing the wrong thing
 *
 * feels really good to find something "new" that you haven't
 * seen before, even if you don't understand it at all
 *
*/

#define BRICK_SIZE 1.25

#include <stdlib.h>
#include <math.h>
#include "world.h"
#include "collision.h"
#include "math/perlin.h"
#include "particles/particle-system.h"
#include "graphics/static_mesh.h"
#include "object/level-platform.h"

World::World()
{
#ifdef __GAME__
    render_shepherd = NULL;
	player = new Player2D();
	level = new Level(this);

	pet = new PlayerPet;

	//init Box2D Physics
	b2d_world = NULL;
  	b2Vec2 gravity;
  	gravity.Set(0.0f, -25.0f);
  	b2d_world = new b2World(gravity);
	assert(b2d_world);
#endif //__GAME__
}

World::~World()
{
#ifdef __GAME__
	if(player)		{ delete player; }
	if(level)		{ delete level; }
	if(pet)			{ delete pet; }

	if(b2d_world)	{ delete b2d_world; }
#endif //__GAME__
}

void World::init()
{
#ifdef __GAME__
	player->set_b2d_world(b2d_world);
	pet->set_b2d_world(b2d_world);
	level->set_b2d_world(b2d_world);

	//load the level
	char fname[256] = "data/bin/test.pkg";
	bool success = level->read_from_file(fname, render_shepherd);
	assert(success);

	player->init();
	pet->init();
	level->init();
	particle_manager.init();
#endif //__GAME__
}

void World::register_renderer(RenderShepherd *rs)
{
    /*std::vector<LevelPlatform2D *>::iterator lpi;
    assert(level);
    for(lpi = level->platforms.begin(); lpi != level->platforms.end(); lpi++)
    {
        LevelPlatform2D *lp = *lpi;
        assert(lp);

        rs->register_mesh(lp->mesh);
    }*/

    /*std::vector<StaticMesh *>::iterator smi;
    for(smi = level->meshes.begin(); smi != level->meshes.end(); smi++)
    {
        StaticMesh *sm = *smi;
        assert(sm);
        rs->register_mesh(sm);
    }

    std::vector<Material *>::iterator mi;
    for(mi = level->materials.begin(); mi != level->meshes.end(); mi++)
    {
        Material *m = *mi;
        assert(m);
        rs->register_material(m);
    }

    std::vector<Texture *>::iterator ti;
    for(mi = level->textures.begin(); ti != level->textures.end(); ti++)
    {
        Texture *t = *ti;
        assert(t);
        rs->register_texture(t);
    }*/
}

void World::simulate(float frame_time, float game_time)
{
    std::vector<Float2>::iterator i;
    for(i = plant_pos_queue.begin(); i != plant_pos_queue.end(); i++)
    {
        Float2 p = *i;

        Plant *pp = new Plant;
        assert(pp);
        pp->set_pos(p);
        pp->set_b2d_world(b2d_world);
        pp->set_world(this);
        pp->init();

        plants.push_back(pp);
    }
    plant_pos_queue.clear();
#ifdef __GAME__
	//update box2D physics
	b2d_world->Step(frame_time, 4, 2);

	level->simulate(frame_time, game_time);
	player->simulate(frame_time);
	pet->simulate(frame_time);
	particle_manager.simulate(frame_time);

	std::vector<Plant *>::iterator pi;
	for(pi = plants.begin(); pi != plants.end(); pi++)
	{
	    Plant *pp = (*pi);
	    assert(pp);
        pp->simulate(frame_time);
    }

	//respawn
	if(player->pos[1] < -50.0f) {
		player->pos[0] = 0.0f;
		player->pos[1] = 0.5f;
		player->vel[0] = 0.0f;
		player->vel[1] = 0.0f;

		b2Vec2 v(0.0f, 0.0f);
		player->b2d_body->SetLinearVelocity(v);
		b2Vec2 p(0.0f, 0.5f);
		player->b2d_body->SetTransform(p, 0.0f);
	}

	//update pet
	{
		float a[2] = { 0.0f, 0.0f };
		a[0] = player->pos[0] - pet->pos[0];
		a[1] = player->pos[1] - pet->pos[1];

		float dist = sqrt(a[0] * a[0] + a[1] * a[1]);
		a[0] /= dist;
		a[1] /= dist;
		pet->vel[0] += 0.05f * a[0] / (dist);
		pet->vel[1] += 0.05f * a[1] / (dist);

		//drag
		pet->vel[0] -= pet->vel[0] * 0.05f * dist;
		pet->vel[1] -= pet->vel[1] * 0.05f * dist;

		pet->vel[0] = pet->vel[0] > 0.2f ? 0.2f	: pet->vel[0];
		pet->vel[0] = pet->vel[0] < -0.2f ? -0.2f : pet->vel[0];
		pet->vel[1] = pet->vel[1] > 0.2f ? 0.2f	: pet->vel[1];
		pet->vel[1] = pet->vel[1] < -0.2f ? -0.2f : pet->vel[1];

		//update pos
		pet->pos[0] += pet->vel[0] * 5.0f;
		pet->pos[1] += pet->vel[1] * 5.0f;

		if(dist > 20.0f) {
			pet->pos[0] = player->pos[0] + 1.0f;
			pet->pos[1] = player->pos[1] + 1.0f;
		}
		if(dist < 5.0f) {
			pet->vel[0] = -pet->vel[0];
			pet->vel[1] = -pet->vel[1];
		}
	}
#endif //__GAME__
}

void World::render_gl()
{
#ifdef __GAME__
	player->render_gl();
	pet->render_gl();
#endif //__GAME__

	//level->render_gl();

	std::vector<Plant *>::iterator pi;
	for(pi = plants.begin(); pi != plants.end(); pi++)
	{
	    Plant *pp = (*pi);
	    pp->render_gl();
	}

	level->render_gl();

	//loaded_physics_shapes[0]->render_gl();

#ifdef __GAME__
	particle_manager.render_gl();
#endif //__GAME__
}

#ifdef __GAME__
void World::set_player(Player2D *p)
{
	if(player != NULL) {
		delete player;
	}
	player = p;
}

Player2D *World::get_player()
{
	return player;
}
#endif //__GAME__

void World::set_level(Level *l)
{
	if(level != NULL) {
		delete level;
	}
	level = l;
}

Level *World::get_level()
{
	return level;
}

void World::set_render_shepherd(RenderShepherd *rs)
{
    render_shepherd = rs;
}

StaticMesh *World::get_mesh(int idx)
{
    return NULL;//loaded_meshes[idx];
}

#ifdef __GAME__
void World::drop_seed(Float2 p)
{
    /*Plant *pp = new Plant;
    assert(pp);
    pp->set_pos(p);
    pp->set_b2d_world(b2d_world);
    pp->set_world(this);
    pp->init();

    plants.push_back(pp);*/

    plant_pos_queue.push_back(p);
}

void World::shit_a_brick()
{
    //return; //crates seem to be broken (rendering?)
	player->pos;
	if(level->crates.size() < (MAX_LEVEL_CRATES - 1)) {
		float d = BRICK_SIZE * (0.5f * (float)(rand() % 10) / 10.0f + 0.1f);
		float c_dim[2] = { d, d * 4.0f / 3.0f };
		float c_pos[2] = { player->pos[0], player->pos[1] - 1.0f };
		Crate2D *cp = new Crate2D(c_pos, c_dim);
		cp->set_b2d_world(b2d_world);
		//cp->set_mesh(loaded_meshes[0]);
		cp->init();

		b2Vec2 vel(0.0f, 13.0f);
		cp->get_b2d_body()->SetLinearVelocity(vel);
		cp->get_b2d_body()->SetAngularVelocity((float)(rand() % 10 - 5) * 5.0f);

		//level->crates[level->crates.size()] = cp;
		//level->num_crates++;
		level->crates.push_back(cp);
	}
}

void World::clear_crates()
{
	for(int i = 0; i < level->crates.size(); i++) {
		delete level->crates[i];
	}
	//level->num_crates = 0;
	level->crates.clear();
}

void World::blast()
{
    ParticleSystem *ps = particle_manager.allocate_particle_system();
    //assert(ps);

    b2Vec2 player_pos = player->get_b2d_body()->GetPosition();

    if(ps)
    {
        ParticleSystemTemplate pst;
        pst.flags = 0;
        pst.duration = 0.1f;
        pst.emission_rate = 0.0f;//4000.0f;

        pst.initial_particle_count = 100;

        pst.particle_lifespan[0] = 0.5f;
        pst.particle_lifespan[1] = 2.0f;

        pst.tint_start[0] = 1.0f;
        pst.tint_start[1] = 0.0f;
        pst.tint_start[2] = 0.0f;

        pst.tint_end[0] = 0.05f;
        pst.tint_end[1] = 0.08f;
        pst.tint_end[2] = 0.03f;

        pst.scale_start[0] = 0.5f;
        pst.scale_start[1] = 0.5f;

        pst.scale_end[0] = 0.1f;
        pst.scale_end[1] = 0.1f;

        pst.texture = loaded_textures[rand() % 3];

        ps->init(&pst);

        float system_pos[3];
        system_pos[0] = player_pos.x;
        system_pos[1] = player_pos.y;
        system_pos[2] = 0.0f;

        particle_manager.run_particle_system(ps, system_pos);
    }

	for(int i = 0; i < level->crates.size(); i++) {
		Crate2D *cp = level->crates[i];
		b2Vec2 force_dir = cp->get_b2d_body()->GetPosition() - player_pos;
		float crate_dist_squared = force_dir.LengthSquared();
		force_dir.Normalize();

		float F = 15.0f / crate_dist_squared;
		force_dir *= F;

		cp->get_b2d_body()->ApplyForceToCenter(force_dir, true);
	}
}

void World::attract()
{
    std::vector<Plant *> to_delete;
    std::vector<Plant *>::iterator pi;
    for(pi = plants.begin(); pi != plants.end(); pi++) {
        Plant *p = *pi;

		b2Body *b2_body = p->get_b2d_body();
		if(b2_body)
		{
            b2Vec2 force_dir = player->get_b2d_body()->GetPosition() - b2_body->GetPosition();
            float dist = force_dir.Length();
            force_dir.Normalize();

            float F = 3.0f / dist;
            force_dir *= F;

            if(dist > 3.0f)
            {
                p->get_b2d_body()->ApplyForceToCenter(force_dir, true);
            }
            else
            {
                //add to inventory
                player->set_num_seeds(player->get_num_seeds() + 1);
                to_delete.push_back(p);
            }
		}
	}

	//now delete all the seeds we need to
	for(pi = to_delete.begin(); pi != to_delete.end(); pi++)
	{
	    Plant *p1 = *pi;
	    std::vector<Plant *>::iterator pj;
	    for(pj = plants.begin(); pj != plants.end(); pj++)
	    {
	        Plant *p2 = *pj;
	        if(p1 == p2)
	        {
	            plants.erase(pj);
	            delete p1;
	            break;
	        }
	    }
	}

}

#endif //__GAME__
