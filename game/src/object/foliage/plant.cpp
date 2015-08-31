#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include "graphics/glutility.h"
#include "math/math-utility.h"

#include "plant.h"
#include "world.h"

using namespace std;

Plant::Plant() : Object2D()
{
    age = 0.0f;
    for(int i = 0; i < NUM_PLANT_LIFECYCLES; i++)
    {
        life_cycle_ages[i] = 0.0f;
    }

    life_cycle_state = PLANT_LIFECYCLE_SEED;
    fruiting_rate = 0.0f;
    time_since_last_fruit = 0.0f;

    float d = Math::random(0.1f, 0.2f);
    dim[0] = d;
    dim[1] = d;

    tmp_mat = new Material;
    tmp_mat->set_diff_rgb(Float3(0.7f, 0.5f, 0.2f));
    tmp_mat->enable_lighting(true);
    tmp_mat->enable_blending(false);
}

Plant::~Plant()
{
    if(b2d_body)
    {
        b2d_world->DestroyBody(b2d_body);
        b2d_body = NULL;
    }

    if(tmp_mat)
    {
        delete tmp_mat;
    }
}

void Plant::render_gl()
{
    //glUseProgramObjectARB(0);
    //glDisable(GL_LIGHTING);



    //dumb.
    float _pos[3] = { pos[0], pos[1], pos[2] };
    float _dim[3] = { dim[0], dim[1], dim[2] };

    switch(life_cycle_state)
    {

        case PLANT_LIFECYCLE_SEED:
            tmp_mat->set_diff_rgb(Float3(0.5f, 0.2f, 0.0f));
            //glColor3f(0.0f, 0.0f, 0.0f);
            break;
        case PLANT_LIFECYCLE_SHRUB:
            tmp_mat->set_diff_rgb(Float3(0.0f, 1.0f, 0.0f));
            break;
        case PLANT_LIFECYCLE_MATURE:
            tmp_mat->set_diff_rgb(Float3(0.0f, 1.0f, 0.5f));
            break;
        case PLANT_LIFECYCLE_DYING:
            tmp_mat->set_diff_rgb(Float3(0.5f, 0.5f, 0.5f));
            break;
        default:
            tmp_mat->set_diff_rgb(Float3(1.0f, 0.0f, 0.0f));
            break;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
    glPushMatrix();

        tmp_mat->render_gl();

        if(life_cycle_state == PLANT_LIFECYCLE_SEED)
        {
            gl_sphere(_pos, _dim, 1.0f);
        }
        else
        {
            gl_cube(_pos, _dim);
        }

    glPopMatrix();
    glPopAttrib();
}

void Plant::set_world(World *w)
{
    world = w;
}

void Plant::init()
{
    tmp_mat->init();

    age = 0.0f;
    life_cycle_ages[PLANT_LIFECYCLE_SEED] = Math::random(5.0f, 10.0f);
    life_cycle_ages[PLANT_LIFECYCLE_SHRUB] = Math::random(25.0f, 30.0f);
    life_cycle_ages[PLANT_LIFECYCLE_MATURE] = Math::random(50.0f, 60.0f);
    life_cycle_ages[PLANT_LIFECYCLE_DYING] = Math::random(350.0f, 400.0f);

    //this should probably be a curve, really, rising as the plant
    //matures, and then falling off to 0 as it dies.
    fruiting_rate = Math::random(0.1f, 0.2f);

    life_cycle_state = PLANT_LIFECYCLE_SEED;

    //physics shit
#ifdef __GAME__
	assert(b2d_world);

	//initialize the box2D physics body
	b2BodyDef body_def;
	body_def.userData = this;
	body_def.type = b2_dynamicBody;
	body_def.position.Set(pos[0], pos[1]);
	body_def.allowSleep = true;
	body_def.linearDamping = 0.0f;
	body_def.angularDamping = 4.0f;
	body_def.fixedRotation = false;
	b2d_body = b2d_world->CreateBody(&body_def);
	assert(b2d_body);

    b2CircleShape dynamic_circle;
    //dynamic_circle.m_p.Set(2.0f, 3.0f);
    dynamic_circle.m_radius = dim[0];
	//b2PolygonShape dynamic_box;
	//dynamic_box.SetAsBox(dim[0] / 2.0f, dim[1] / 2.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixture_def;
	//fixture_def.shape = &dynamic_box;
	fixture_def.shape = &dynamic_circle;
	fixture_def.density = 0.1f;
	fixture_def.friction = 1.0f;
	fixture_def.restitution = 0.45f;

	// Add the shape to the body.
	b2d_body->CreateFixture(&fixture_def);
#endif //__GAME__
}

void Plant::simulate(const float t)
{
    age += t;

    //cout<<age<<endl;
    if(age > life_cycle_ages[PLANT_LIFECYCLE_SEED])
    {
        life_cycle_state = PLANT_LIFECYCLE_SHRUB;

        //can no longer move, and we don't want to deal with physics any more
        if(b2d_body)
        {
            assert(b2d_world);
            b2d_world->DestroyBody(b2d_body);
            b2d_body = NULL;
        }
    }
    if(age > life_cycle_ages[PLANT_LIFECYCLE_SHRUB])
    {
        life_cycle_state = PLANT_LIFECYCLE_MATURE;
    }
    if(age > life_cycle_ages[PLANT_LIFECYCLE_MATURE])
    {
        life_cycle_state = PLANT_LIFECYCLE_DYING;
    }
    if(age > life_cycle_ages[PLANT_LIFECYCLE_DYING])
    {
        //dead....
        //clean up
    }

    if(life_cycle_state == PLANT_LIFECYCLE_SHRUB)
    {
        float growth_rate = 0.12f;
        dim[1] += t * growth_rate;
        dim[0] += t * growth_rate * 0.01f;
        pos[1] += (t * growth_rate) * 0.5f;
    }

    //maybe fruit
    if(life_cycle_state == PLANT_LIFECYCLE_MATURE)
    {
        int num_seeds = (int)(fruiting_rate * time_since_last_fruit);
        if(num_seeds > 0) {
            time_since_last_fruit = 0.0f;
        }
        else
        {
            time_since_last_fruit += t;
        }

        assert(world);
        for(int i = 0; i < num_seeds; i++)
        {
            Float2 drop_pos(pos);
            drop_pos[1] += 3.0f;
            drop_pos[0] += Math::random(-1.0f, 1.0f);
            world->drop_seed(drop_pos);
            cout<<"plant::simulate::drop_seed()"<<endl;
        }
    }

    //query physics
    if(b2d_body)
    {
        b2Vec2 body_pos = b2d_body->GetPosition();
        pos[0] = body_pos.x;
        pos[1] = body_pos.y;
    }
}
