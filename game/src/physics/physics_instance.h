#ifndef __PHYSICS_INSTANCE_H__
#define __PHYSICS_INSTANCE_H__

#include "physics/physics_shape.h"

class PhysicsInstance2D
{
    public:
        PhysicsInstance2D();
        ~PhysicsInstance2D();

        bool init();

        void set_shape(PhysicsShape2D *s);
        PhysicsShape2D *get_shape();

        Float2 get_position() const;
        Float2 get_velocity() const;

        void set_position(const Float2 &p);
        void set_velocity(const Float2 &v);

#ifdef __GAME__
        void set_b2d_world(b2World *w);
#endif //__GAME__

        void render_gl();
    private:
        PhysicsShape2D *shape;

        Float2 pos;
        Float2 vel;

        //float restitution;
        //float friction;

#ifdef __GAME__
        b2World *b2d_world;
        b2Body *b2d_body;
#endif //__GAME__

};

#endif // __PHYSICS_INSTANCE_H__
