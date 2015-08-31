#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "graphics/glutility.h"
#include "math/math-utility.h"
#include "object/crate.h"

using namespace std;

Crate2D::Crate2D() : Object2D()
{
	object_type = OBJECT_CRATE;
	mesh = NULL;
}

Crate2D::Crate2D(float *_pos, float *_dim) : Object2D(_pos, _dim)
{
	object_type = OBJECT_CRATE;
	mesh = NULL;
}

Crate2D::~Crate2D() {
	//b2d_body->DestroyFixture();
#ifdef __GAME__
	b2d_world->DestroyBody(b2d_body);
#endif //__GAME__
}

void Crate2D::init()
{
#ifdef __GAME__
	assert(b2d_world);

	//initialize the box2D physics body
	b2BodyDef body_def;
	body_def.userData = this;
	body_def.type = b2_dynamicBody;
	body_def.position.Set(pos[0], pos[1]);
	body_def.allowSleep = true;
	body_def.linearDamping = 0.0f;
	body_def.fixedRotation = false;
	b2d_body = b2d_world->CreateBody(&body_def);
	assert(b2d_body);

	b2PolygonShape dynamic_box;
	dynamic_box.SetAsBox(dim[0] / 2.0f, dim[1] / 2.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixture_def;
	fixture_def.shape = &dynamic_box;
	fixture_def.density = 0.1f;
	fixture_def.friction = 1.0f;
	fixture_def.restitution = 0.45f;

	// Add the shape to the body.
	b2d_body->CreateFixture(&fixture_def);
#endif //__GAME__
}

void Crate2D::simulate(const float t)
{
#ifdef __GAME__
	assert(b2d_body);
	b2Vec2 body_pos = b2d_body->GetPosition();
	pos[0] = body_pos.x;
	pos[1] = body_pos.y;
#endif //__GAME__
}

#ifdef __EDITOR__
bool Crate2D::write_to_file(ofstream &f)
{
    f<<"\t<crate>"<<endl;
    f<<"\t\t<pos>"<<pos[0]<<" "<<pos[1]<<"</pos>"<<endl;
    f<<"\t\t<scale>"<<dim[0]<<" "<<dim[1]<<"</scale>"<<endl;
    f<<"\t</crate>"<<endl<<endl;

    return true;
}
#endif //__EDITOR__

void Crate2D::render_gl()
{
    glUseProgramObjectARB(0);
    glPushMatrix();

    float angle = 0.0f;
#ifdef __GAME__
    angle = 180.0f * b2d_body->GetAngle() / M_PI;
#endif //__GAME__
    /*
    glTranslatef(pos[0], pos[1], 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glRotatef(pos[0] * 50.0f, 0.0f, 1.0f, 0.0f);

    glScalef(dim[0], dim[1], (dim[0] + dim[1]) / 2.0f);
    */

    float lerp_val = 0.0f;
#ifdef __GAME__
    b2Vec2 vel = b2d_body->GetLinearVelocity();
    lerp_val = Math::min(0.1f * vel.Length() + 0.2f, 1.0f);
#endif //__GAME__
    float r = Math::lerp(0.25f, 1.0f, lerp_val * 1.5f);
    float g = Math::lerp(0.0f, 0.9f, lerp_val);
    float b = Math::lerp(0.0f, 0.8f, lerp_val * 0.3f);

    //glColor3f(r, g, b);
    GLfloat emit[4] = { r, g, b, 1.0f };
    GLfloat diff[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat spec[4] = { 0.2f, 0.2f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_EMISSION, emit);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

    float render_pos[3] = { pos[0], pos[1], 0.0f };
    float render_dim[3] = { dim[0], dim[1], dim[1] };
    gl_cube(render_pos, render_dim, 180.0f * b2d_body->GetAngle() / M_PI);
    //mesh->render_gl();
    glPopMatrix();
    return;


	glPushMatrix();
	{
		float render_pos[3] = { pos[0], pos[1], 0.0f };
		float render_dim[3] = { dim[0], dim[1], dim[1] };

        float lerp_val = 0.0f;
#ifdef __GAME__
		b2Vec2 vel = b2d_body->GetLinearVelocity();
		lerp_val = Math::min(0.1f * vel.Length() + 0.2f, 1.0f);
#endif //__GAME__

		float r = Math::lerp(0.25f, 1.0f, lerp_val * 1.5f);
		float g = Math::lerp(0.0f, 0.9f, lerp_val);
		float b = Math::lerp(0.0f, 0.8f, lerp_val * 0.3f);

		//glColor3f(r, g, b);
		GLfloat emit[4] = { r, g, b, 1.0f };
		GLfloat diff[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		GLfloat spec[4] = { 0.2f, 0.2f, 0.3f, 1.0f };

		glMaterialfv(GL_FRONT, GL_EMISSION, emit);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

#ifdef __GAME__
		assert(b2d_body);
		gl_cube(render_pos, render_dim, 180.0f * b2d_body->GetAngle() / M_PI);
#endif //__GAME__

#ifdef __EDITOR__
        glColor3f(1.0f, 1.0f, 0.0f);
        gl_cube(render_pos, render_dim, 0.0f);
#endif
	}
	glPopMatrix();
}

bool Crate2D::hit_test_2d(float p[2])
{
    return false;
}
