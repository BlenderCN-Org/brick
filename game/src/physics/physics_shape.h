#ifndef __PHYSICS_SHAPE_H__
#define __PHYSICS_SHAPE_H__

#ifdef __GAME__
#include <Box2D/Box2D.h>
#endif //__GAME__

#include "math/math-utility.h"

using namespace Math;

typedef struct
{
    int hash_id;
    int num_verts;

    Float2 *verts;
} PhysicsShapeFileData;

class PhysicsShape2D
{
    public:
        PhysicsShape2D();
        ~PhysicsShape2D();

        bool read_from_file(const char *fname);
        bool init();

        void set_vertices(int count, Float2 *p_verts);
        Float2 *get_vertices(int &count);

        void render_gl();
    protected:
        int num_verts;
        Float2 *shape_verts;
};

#endif //__PHYSICS_SHAPE_H__
