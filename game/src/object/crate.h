#ifndef _CRATE_H_
#define _CRATE_H_

#include <fstream>

#include "graphics/static_mesh.h"

#include "object.h"

using namespace std;

class Crate2D: public Object2D
{
friend class Level;
public:
	Crate2D();
	Crate2D(float *_pos, float *_dim);
	~Crate2D();

	void init();
	void simulate(const float t);

#ifdef __EDITOR__
	bool write_to_file(ofstream &f);
#endif //__EDITOR__

	void render_gl();

	bool hit_test_2d(float p[2]);
private:

};

#endif // _CRATE_H_
