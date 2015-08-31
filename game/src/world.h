#ifndef _WORLD_H_
#define _WORLD_H_

#include <Box2D/Box2D.h>

#include "graphics/render_shepherd.h"
#include "level.h"
#include "object/player.h"
#include "object/player-pet.h"
#include "particles/particle-manager.h"
#include "physics/physics_shape.h"
#include "object/foliage/plant.h"

class World
{
friend class Level;
public:
	World();
	~World();

	void init();
	void register_renderer(RenderShepherd *rs);

	void simulate(float frame_time, float game_time);
	void render_gl();

	void set_level(Level *l);
	Level *get_level();

	void set_render_shepherd(RenderShepherd *rs);

	StaticMesh *get_mesh(int idx);

	//super tempy / debugy
#ifdef __GAME__
	void set_player(Player2D *p);
	Player2D *get_player();

    void drop_seed(Float2 p);
	void shit_a_brick();
	void clear_crates();
	void blast();
	void attract();
#endif //__GAME

private:

#ifdef __GAME__
    RenderShepherd *render_shepherd;
	ParticleManager particle_manager;
	Player2D *player;
	PlayerPet *pet;

	b2World *b2d_world;
#endif //__GAME__

    Level *level;

    //tmp
    std::vector<Plant *> plants;

    std::vector<Texture *> loaded_textures;

	//std::vector<StaticMesh *> loaded_meshes;
	//std::vector<PhysicsShape2D *> loaded_physics_shapes;

	//replace with message system?
	std::vector<Float2> plant_pos_queue;
};

#endif // _WORLD_H_
