#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GL/gl.h>

#include "static_mesh.h"

class Texture
{
public:
	Texture();
	Texture(const char *n);

	~Texture();

	bool load();
	bool load_from_file_data(TextureFileData &tfd);
	bool render_gl() const;

private:
	int			dim[2];
	char		fname[256];
	GLuint		gl_texture;
};

#endif // _TEXTURE_H_