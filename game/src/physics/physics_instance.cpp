#include "physics_instance.h"

PhysicsInstance2D::PhysicsInstance2D()
{
    shape = NULL;

#ifdef __PHYSICS__
    b2d_world = NULL;
    b2d_body = NULL;
#endif //__PHYSICS__
}

PhysicsInstance2D::~PhysicsInstance2D()
{

}

bool PhysicsInstance2D::init()
{
    //do box2D stuff
#ifdef __PHYSICS__
    assert(b2d_world);
    assert(shape);

	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef body_def;
	body_def.type = b2_kinematicBody;//b2_dynamicBody;
	body_def.position.Set(0.0f, 0.0f);//pos[0], pos[1]);
	body_def.angle = 0.0f;//theta;
	body_def.userData = this;
	b2d_body = b2d_world->CreateBody(&body_def);
	assert(b2d_body);

    //create the b2d shape from the PhysicsShape2D verts
    //Note: we are allocating a separate and unique
    //b2d shape for each physics instance, even though
    //there are likely shared shapes (optimization possible)
    int num_verts;
    Float2 *shape_verts = shape->get_vertices(num_verts);
    b2Vec2 *b2d_verts = new b2Vec2[num_verts];
    for(int i = 0; i < num_verts; i++)
    {
        b2d_verts[i].x = shape_verts[i][0];
        b2d_verts[i].y = shape_verts[i][1];
    }
    b2PolygonShape b2d_shape;
    b2d_shape.Set(b2d_verts, num_verts);

    b2FixtureDef b2d_fixture_def;
    b2d_fixture_def.shape = &b2d_shape;
    b2d_fixture_def.restitution = 0.2f;
    b2d_fixture_def.friction = 1.0f;

    b2d_body->CreateFixture(&b2d_fixture_def);

    delete b2d_verts;
    return true;
#endif //__PHYSICS__
}

void PhysicsInstance2D::set_shape(PhysicsShape2D *s)
{
    shape = s;
}

PhysicsShape2D *PhysicsInstance2D::get_shape()
{
    return shape;
}

void PhysicsInstance2D::set_b2d_world(b2World *w)
{
    b2d_world = w;
}

void PhysicsInstance2D::set_position(const Float2 &p)
{
    if(b2d_world && b2d_body)
    {
        b2Vec2 pos;
        pos.x = p[0];
        pos.y = p[1];
        b2d_body->SetTransform(pos, 0.0f);
    }
}
