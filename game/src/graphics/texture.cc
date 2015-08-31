#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <SOIL/SOIL.h>

#include <iostream>

#include <assert.h>

#include "texture.h"

using namespace std;

Texture::Texture()
{
	fname[0] = '\0';
	gl_texture = 0;
	dim[0] = dim[1] = -1;
}

Texture::Texture(const char *n)
{
	strcpy(fname, n);
	gl_texture = 0;
	dim[0] = dim[1] = -1;
}

Texture::~Texture() {}

bool Texture::load()
{
    if(gl_texture != 0)
    {
        assert(glIsTexture(gl_texture) == GL_TRUE);
        return true; //already loaded
    }

	glGenTextures(1, &gl_texture);

	glBindTexture(GL_TEXTURE_2D, gl_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if(glIsTexture(gl_texture) != GL_TRUE)
	{
	    assert(false);
	    return false;
	}

    int width, height, channels;
    unsigned char *image = SOIL_load_image("/home/chandra/brick/game/data/textures/cf_wtr_drop_01.tga", &width, &height, &channels, SOIL_LOAD_RGBA);
    assert(image);

    dim[0] = width;
    dim[1] = height;

    //not sure if these should go here, or in the render loop
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Filtering


    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image);

    SOIL_free_image_data(image);

	return true;
}

bool Texture::load_from_file_data(TextureFileData &tfd)
{
    if(gl_texture != 0)
    {
        assert(glIsTexture(gl_texture) == GL_TRUE);
        return true; //already loaded
    }

	glGenTextures(1, &gl_texture);

	glBindTexture(GL_TEXTURE_2D, gl_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if(glIsTexture(gl_texture) != GL_TRUE)
	{
	    assert(false);
	    return false;
	}

    dim[0] = tfd.dim[0];
    dim[1] = tfd.dim[1];

    //not sure if these should go here, or in the render loop
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering

    cout<<"glTexImage2D()"<<endl;
    cout<<"width: "<<dim[0]<<endl;
    cout<<"height: "<<dim[1]<<endl;

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 dim[0],
                 dim[1],
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 tfd.image);

    return true;
}

//set up the texture for rendering
bool Texture::render_gl() const
{
    glEnable(GL_TEXTURE_2D);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBindTexture(GL_TEXTURE_2D, gl_texture);

	return true;
}
