#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif //__APPLE__

#include <vector>

#include "../math/math-utility.h"
#include "shader.h"
//#include "texture.h"

using namespace Math;

class Material {
    public:
        Material();
        ~Material();

        void init();

        void enable_lighting(const bool l);
        void enable_blending(const bool b);

        void set_blend_mode(const GLenum src, const GLenum dst);
        void set_alpha(const float a);
        void set_diff_rgb(const Float3 rgb);
        void set_spec_rgb(const Float3 rgb);
        void set_emit_rgb(const Float3 rgb);

        void render_gl() const;

        void add_tex_idx(const int tid);

    private:
        bool lighting;
        bool transparent;

        GLenum src_blend_param;
        GLenum dst_blend_param;

        float alpha;

        Float3 diff_rgb;
        Float3 spec_rgb;
        Float3 emit_rgb;

        //std::vector<Texture *> textures;
        std::vector<int> texture_ids;
        Shader *shader;
};

#endif // __MATERIAL_H__
