#include <assert.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "math/math-utility.h"

#include "particle-system.h"
#include "particle-manager.h"

ParticleSystem::ParticleSystem()
{
	emitter_shape = EMITTER_SHAPE_SPHERE;
	emission_rate = 5.0f;
	lifespan[0] = 0.5f;
	lifespan[1] = 1.0f;

	max_age = 1.0f;
	age_left = 1.0f;
	flags = 0;

	tint_start[0] = 0.8f;
	tint_start[1] = 0.5f;
	tint_start[2] = 0.2f;

	tint_end[0] = 0.0f;
	tint_end[1] = 0.0f;
	tint_end[2] = 0.0f;

	scale_start[0] = scale_start[1] = 0.05f;
	scale_end[0] = scale_end[1] = 0.3f;

	system_pos[0] = system_pos[1] = system_pos[2] = 0.0f;

	//gl stuff
	gl_vbo = -1;
	gl_ibo = -1;

	//tex = new Texture;
	tex = NULL;
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init_gl()
{
	/*glGenBuffersARB(1, &gl_vbo);
	assert(gl_vbo != -1);

	glGenBuffersARB(1, &gl_ibo);
	assert(gl_ibo != -1);
	*/

	//tex->load();
}

void ParticleSystem::render_gl()
{
	/*GLfloat verts[GL_RENDER_BATCH_SIZE * 12];

	std::vector<Particle *>::iterator it = particle_list.begin();
	int vi = 0;
	while(vi < GL_RENDER_BATCH_SIZE && it != particle_list.end()) {
		Particle *p = *it;
		assert(p);

		float p_color[3] = { 0.0f, 0.0f, 0.0f };
		float lerp_val = 1.0f - p->age_left / 2.0f;
		p_color[0] = Math::cerp(tint_start[0], tint_end[0], lerp_val);
		p_color[1] = Math::cerp(tint_start[1], tint_end[1], lerp_val);
		p_color[2] = Math::cerp(tint_start[2], tint_end[2], lerp_val);
		glColor3f(p_color[0], p_color[1], p_color[2]);

		float half_width = Math::cerp(scale_start[0], scale_end[0], lerp_val);
		float half_height = Math::cerp(scale_start[1], scale_end[1], lerp_val);


		//top left
		verts[vi++] = p->pos[0] - half_width;
		verts[vi++] = p->pos[1] + half_height;
		verts[vi++] = p->pos[2];

		//top right
		verts[vi++] = p->pos[0] + half_width;
		verts[vi++] = p->pos[1] + half_height;
		verts[vi++] = p->pos[2];

		//bottom right
		verts[vi++] = p->pos[0] + half_width;
		verts[vi++] = p->pos[1] - half_height;
		verts[vi++] = p->pos[2];

		//bottom left
		verts[vi++] = p->pos[0] - half_width;
		verts[vi++] = p->pos[1] - half_height;
		verts[vi++] = p->pos[2];

		it++;	//next particle
	}



	//glVertexPointer()
	//glColorPointer()
	//glTexCoordPointer()
	//glDrawElements()

	return;*/
	//terrible terrible terrible
	//at least use vertex arrays... C'MON!!!!
	glUseProgramObjectARB(0);
	glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	//glShadeModel(GL_FLAT);

	glPushMatrix();
	glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);

    assert(tex);
	tex->render_gl();

	glBegin(GL_QUADS);
	{
		std::vector<Particle *>::iterator it;
		for(it = particle_list.begin(); it != particle_list.end(); it++) {
			Particle *p = *it;

			float p_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			float lerp_val = 1.0f - (p->age_left / p->max_age);
			p_color[0] = Math::cerp(tint_start[0], tint_end[0], lerp_val);
			p_color[1] = Math::cerp(tint_start[1], tint_end[1], lerp_val);
			p_color[2] = Math::cerp(tint_start[2], tint_end[2], lerp_val);
			glColor4f(p_color[0], p_color[1], p_color[2], Math::cerp(1.0f, 0.0f, lerp_val));

			float half_width = Math::cerp(scale_start[0], scale_end[0], lerp_val);
			float half_height = Math::cerp(scale_start[1], scale_end[1], lerp_val);

            glTexCoord2f(0.0f, 0.0f);
			glVertex3f(p->pos[0] - half_width, p->pos[1] + half_height, p->pos[2]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(p->pos[0] + half_width, p->pos[1] + half_height, p->pos[2]);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(p->pos[0] + half_width, p->pos[1] - half_height, p->pos[2]);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(p->pos[0] - half_width, p->pos[1] - half_height, p->pos[2]);
		}
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void ParticleSystem::init()
{
	init_gl();
}

void ParticleSystem::init(const ParticleSystemTemplate *pst)
{
    flags = pst->flags;
    max_age = pst->duration;
    age_left = pst->duration;

    emission_rate = pst->emission_rate;
    initial_particle_count = pst->initial_particle_count;

    lifespan[0] = pst->particle_lifespan[0];
    lifespan[1] = pst->particle_lifespan[1];

    tint_start[0] = pst->tint_start[0];
    tint_start[1] = pst->tint_start[1];
    tint_start[2] = pst->tint_start[2];

    tint_end[0] = pst->tint_end[0];
    tint_end[1] = pst->tint_end[1];
    tint_end[2] = pst->tint_end[2];

    scale_start[0] = pst->scale_start[0];
    scale_start[1] = pst->scale_start[1];

    scale_end[0] = pst->scale_end[0];
    scale_end[1] = pst->scale_end[1];

    tex = pst->texture;

    init_gl();
}
