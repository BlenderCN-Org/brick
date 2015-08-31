#include "collision.h"


bool Collision::aa_rect(const float *r1_pos,
                        const float *r1_dim,
                        const float *r2_pos,
                        const float *r2_dim)
{
	float r1_left =		r1_pos[0] - r1_dim[0] / 2.0f;
	float r1_right =	r1_pos[0] + r1_dim[0] / 2.0f;
	float r1_up =		r1_pos[1] + r1_dim[1] / 2.0f;
	float r1_down =		r1_pos[1] - r1_dim[1] / 2.0f;

	float r2_left =		r2_pos[0] - r2_dim[0] / 2.0f;
	float r2_right =	r2_pos[0] + r2_dim[0] / 2.0f;
	float r2_up =		r2_pos[1] + r2_dim[1] / 2.0f;
	float r2_down =		r2_pos[1] - r2_dim[1] / 2.0f;

	if(r1_right < r2_left || r1_left > r2_right ||
	   r1_up < r2_down || r1_down > r2_up)
	{
		return false;
	}
	return true;
}

