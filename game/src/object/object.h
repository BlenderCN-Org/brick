#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __GAME__
#include <Box2D/Box2D.h>
#endif //__GAME__

#include "math/math-utility.h"
#include "graphics/static_mesh.h"
#include "physics/physics_instance.h"

//object type
enum ObjectType {
	OBJECT_NONE,
	OBJECT_PLAYER,
	OBJECT_CRATE,
	OBJECT_LEVEL_PLATFORM,

	OBJECT_PLANT,

	NUM_OBJECT_TYPES
};

class Thing
{
	public:
		Thing();
};

class Object2D : public Thing
{
public:
	Object2D();
	Object2D(const float *_pos, const float *_dim);
	ObjectType get_object_type() const;

	virtual void render_gl() = 0;

	virtual void init() = 0;
	virtual void simulate(const float t) = 0;

	const void get_pos(float &x, float &y);
	Float2 get_pos() const;
	const void get_scale(float &x, float &y);
	Float2 get_scale() const;
	const void get_vel(float &x, float &y);
	Float2 get_vel() const;

	void set_pos(float x, float y);
	void set_pos(Float2 p);
	void set_scale(float x, float y);
	void set_scale(Float2 s);
	void set_vel(float x, float y);
	void set_vel(Float2 v);

	//reads physics / render data from file
	void read_brick_from_file(const char *file_name);

	void set_mesh(StaticMesh *m);
	StaticMesh *get_mesh();

	void set_physics_instance(PhysicsInstance2D *p);
	PhysicsInstance2D *get_physics_instance();

#ifdef __EDITOR__
	virtual bool hit_test_2d(float p[2]) = 0;
#endif //__EDITOR__

#ifdef __PHYSICS__
	void set_b2d_world(b2World *w);
	b2Body *get_b2d_body();
#endif //__PHYSICS__

protected:
	ObjectType object_type;	//what kind of game object am I?
	bool dynamic;	   		//can the object move?

	Math::Float2 pos;	   	//position in 2D space
	Math::Float2 vel;		//velocity in 2D space
	Math::Float2 dim;	   	//width / height in 2D space

#ifdef __PHYSICS__
	b2World *b2d_world;		//Box2D physics world object
	b2Body *b2d_body;  		//Box2D physics body
#endif //__PHYSICS__

    StaticMesh *mesh;
    PhysicsInstance2D *physics_instance;
};

#endif // _OBJECT_H_
