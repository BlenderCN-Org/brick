#include <math.h>
#include <iostream>
#include <GL/gl.h>
#include "player.h"
#include "graphics/glutility.h"
#include "level.h"
#include "collision.h"
#include "level-platform.h"

using namespace std;

Player2D::Player2D() : Object2D()
{
	object_type = OBJECT_PLAYER;

	ground = NULL;

	num_seeds = 10;
	seed_cooldown_timer = 0.0f;
}

Player2D::~Player2D()
{
	cout<<"Player2D::~Player2D()"<<endl;
}

void Player2D::init()
{
#ifdef __GAME__
	cout<<"Player2D::init()"<<endl;
	assert(b2d_world);

	//initialize the box2D physics body
	b2BodyDef body_def;
	body_def.userData = this;
	body_def.type = b2_dynamicBody;
	body_def.position.Set(0.0f, 0.0f);
	body_def.allowSleep = true;//false;
	body_def.linearDamping = 0.0f;
	body_def.fixedRotation = true;
	b2d_body = b2d_world->CreateBody(&body_def);
	assert(b2d_body);

	b2PolygonShape dynamic_box;
	dynamic_box.SetAsBox(PLAYER_WIDTH / 2.0f, PLAYER_HEIGHT / 2.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixture_def;
	fixture_def.shape = &dynamic_box;
	fixture_def.density = 1.0f;
	fixture_def.friction = 1.0f;
	fixture_def.restitution = 0.15f;

	// Add the shape to the body.
	b2d_body->CreateFixture(&fixture_def);
#endif //__GAME__

    shader.load_link_and_compile();
}

//simulate the player
void Player2D::simulate(const float t)
{
    seed_cooldown_timer -= t;
#ifdef __GAME__
	//cout<<"Player2D::simulate()"<<endl;
	assert(b2d_body);
	b2Vec2 body_pos = b2d_body->GetPosition();
	pos[0] = body_pos.x;
	pos[1] = body_pos.y;

	b2Vec2 body_vel = b2d_body->GetLinearVelocity();
	vel[0] = body_vel.x;
	vel[1] = body_vel.y;

	b2ContactEdge *b2d_contact_edge = b2d_body->GetContactList();
	//assert(b2d_contact_edge);
	if(b2d_contact_edge) {
	//while(b2d_contact_edge->next) {
		b2Body *contact = b2d_contact_edge->other;
		assert(contact);

		Object2D *op = (LevelPlatform2D *)contact->GetUserData();
		assert(op);

		if(op->get_object_type() == OBJECT_LEVEL_PLATFORM) {
			LevelPlatform2D *lp = (LevelPlatform2D *)op;

			b2Vec2 contact_vel = b2d_body->GetLinearVelocity();
			b2Vec2 contact_pos = b2d_body->GetPosition();

			if(body_pos.y + PLAYER_HEIGHT / 2.0f > contact_pos.y) {
				ground = lp;

				//if(lp->dynamic) {
				/*
					b2JointDef joint_def;
					//joint_def.SetAnchorA();
					joint_def.type = e_distanceJoint;
					joint_def.bodyA = b2d_body;
					joint_def.bodyB = contact;
					b2d_world->CreateJoint(&joint_def);
				*/
				//}
			}

			/*
			if(fabs(lp->vel[0]) > 0.1f) {
				contact_vel.x = lp->vel[0];
			}
			if(fabs(lp->vel[1]) > 0.1f) {
				contact_vel.y = lp->vel[1];
			}*/

			//b2d_body->SetLinearVelocity(contact_vel);

			/*b2d_vec = b2d_body->GetPosition();

			b2d_vec.x += ((LevelPlatform2D *)b2d_contact_edge->other->GetUserData())->vel[0] * t;
			b2d_body->SetTransform(b2d_vec, 0.0f);
			*/

			/*b2d_vec = contact->GetLinearVelocity();
			//cout<<"("<<b2d_vec.x<<", "<<b2d_vec.y<<")"<<endl;
			b2d_vec.x *= 0.4f;
			b2d_vec.y *= 0.4f;
			b2d_body->ApplyForceToCenter(b2d_vec); */
		}
	//}
	} else {
		//no contacts
		ground = NULL;
	}
#endif //__GAME__
}

//draw the actual player
void Player2D::render_gl()
{
	glPushMatrix();

	float position[3] = { pos[0], pos[1], 0.0f };
	float dim[3] = { PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH };

	GLfloat diff[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat spec[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
	GLfloat emit[4] = { 0.2f, 0.7f, 1.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT, GL_EMISSION, emit);

	gl_cube(position, dim);

	glPopMatrix();
}

bool Player2D::plant_seed()
{
    if(num_seeds > 0 && seed_cooldown_timer < 0.0f)
    {
        num_seeds--;
        seed_cooldown_timer = 3.0f;
        return true;
    }
    return false;
}

int Player2D::get_num_seeds()
{
    return num_seeds;
}

void Player2D::set_num_seeds(int s)
{
    num_seeds = s;
}
