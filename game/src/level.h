#ifndef _LEVEL_H_
#define _LEVEL_H_

#define MAX_LEVEL_PLATFORMS		110
#define MAX_LEVEL_CRATES		1000

#include <fstream>
#include <vector>

#include <stdio.h>

#include "object/object.h"
#include "object/player.h"
#include "object/crate.h"

class RenderShepherd;

using namespace std;

class Level;
class World;

typedef struct
{
    unsigned long   hash_id;

    int             mesh_idx;
    int             physics_idx;
} BrickFileData;

typedef struct
{
    unsigned int magic;
    unsigned int version;
    unsigned int header_size;
    unsigned int file_size;

    //int num_chunks;
} LevelFileHeader;

typedef struct
{
    int brick_idx;

    Float2 position;
    Float2 scale;
    float angle;
} LevelPlatformFileData;

typedef struct
{
    int num_platforms;
    int num_bricks;
    int num_meshes;
    int num_physics_shapes;
    int num_textures;

    LevelPlatformFileData   *platforms;
    BrickFileData           *bricks;
    StaticMeshFileData      *meshes;
    PhysicsShapeFileData    *physics_shapes;
    TextureFileData         *textures;

    /*
    TODO:
    int             num_resources;
    DataResource *  resources;
    */
} LevelFileData;

class Level
{
friend class World;
#ifdef __EDITOR__
friend class App;
#endif //__EDITOR__
public:
	Level();
	Level(World *w);

	~Level();

	void init();
	void simulate(const float frame_time, const float game_time);
	bool read_from_file(const char *filename, RenderShepherd *rs);

#ifdef __EDITOR__
	bool write_to_file(ofstream &f);

	void add_platform(LevelPlatform2D *lp);
#endif //__EDITOR__

	void render_gl();

#ifdef __GAME__
	void set_b2d_world(b2World *w);
#endif //__GAME__

private:
	std::vector<LevelPlatform2D *> platforms;
	std::vector<Crate2D *> crates;

	World *world;

#ifdef __GAME__
	b2World *b2d_world;
#endif //__GAME__
};

#endif // _LEVEL_H_
