#include "object.h"

Thing::Thing() {}

Object2D::Object2D()
{
	object_type = OBJECT_NONE;

	pos[0] = pos[1] = 0.0f;
	vel[0] = vel[1] = 0.0f;

	dim[0] = dim[1] = 1.0f;
	dynamic = false;

#ifdef __GAME__
	b2d_world = NULL;
	b2d_body = NULL;
#endif //__GAME__

    mesh = NULL;
    physics_instance = NULL;
}

Object2D::Object2D(const float *_pos, const float *_dim)
{
	object_type = OBJECT_NONE;

	pos[0] = _pos[0];
	pos[1] = _pos[1];

	dim[0] = _dim[0];
	dim[1] = _dim[1];

	dynamic = false;

#ifdef __GAME__
	b2d_world = NULL;
	b2d_body = NULL;
#endif //__GAME__

    mesh = NULL;
    physics_instance = NULL;
}

ObjectType Object2D::get_object_type() const
{
	return object_type;
}

const void Object2D::get_pos(float &x, float &y)
{
	x = pos[0];
	y = pos[1];
}

Float2 Object2D::get_pos() const
{
    return pos;
}

const void Object2D::get_scale(float &x, float &y)
{
    x = dim[0];
    y = dim[1];
}

Float2 Object2D::get_scale() const
{
    return dim;
}

const void Object2D::get_vel(float &x, float &y)
{
	x = vel[0];
	y = vel[1];
}

Float2 Object2D::get_vel() const
{
    return vel;
}

void Object2D::set_pos(float x, float y)
{
	pos[0] = x;
	pos[1] = y;
}

void Object2D::set_pos(Float2 p)
{
    pos = p;
}

void Object2D::set_scale(float x, float y)
{
    dim[0] = x;
    dim[1] = y;
}

void Object2D::set_scale(Float2 s)
{
    dim = s;
}
void Object2D::set_vel(float x, float y)
{
	vel[0] = x;
	vel[1] = y;
}

void Object2D::set_vel(Float2 v)
{
    vel = v;
}

void Object2D::set_mesh(StaticMesh *m)
{
    mesh = m;
}

StaticMesh *Object2D::get_mesh()
{
    return mesh;
}

void Object2D::set_physics_instance(PhysicsInstance2D *p)
{
    physics_instance = p;
}

PhysicsInstance2D *Object2D::get_physics_instance()
{
    return physics_instance;
}

#ifdef __GAME__
void Object2D::set_b2d_world(b2World *w)
{
	assert(w);
	b2d_world = w;
}

b2Body *Object2D::get_b2d_body()
{
	return b2d_body;
}
#endif //__GAME__
