#include <GL/gl.h>
#include <math.h>
#include "player-pet.h"
#include "../graphics/glutility.h"

PlayerPet::PlayerPet() : Object2D() 
{
	pos[0] = 5;
	pos[1] = 5;
	vel[0] = vel[1] = 0.0f;
}

void PlayerPet::init()
{

}

void PlayerPet::simulate(const float t)
{
	//vel[0] += 0.3f * sin(t * 200.0f);
}

void PlayerPet::render_gl()
{
	glPushMatrix();
	
	GLfloat diff[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spec[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat emit[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT, GL_EMISSION, emit);

	float rpos[3] = { pos[0], pos[1], 0.0f };
	float rdim[3] = { 0.1f, 0.1f, 0.1f };
	gl_cube(rpos, rdim);
	glPopMatrix();
}
