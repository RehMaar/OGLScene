#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>


#include "shaders.hpp"

class Skybox {
    public:
        int init(GLuint txd_idx);
        void set_wvp(const glm::mat4 & wvp);
        void set_texture_unit(void);
		void activate_textures();
        void enable();
		void deactivate_textures();
        void disable();
        void render();

    private:
       
        int init_texture();
        int init_shaders();
        int init_model();

        Shader s_shader;

        GLuint  s_txr_obj;
		GLuint  s_txr_unit;
        GLuint  s_vbo;
        GLuint  s_vao;
        
        enum uniform {
            WVP,
            SAMPLER,
            UNIFORM_LEN
        };

        struct uniform_descr {
            GLuint idx;
            const char * name;
        } descr[UNIFORM_LEN] = {
			{INVALID_UNIFORM, "wvp"},
			{INVALID_UNIFORM, "skybox"}
        };

        GLfloat s_vertices[108] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
};
