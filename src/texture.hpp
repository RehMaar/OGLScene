#pragma once

#include <GL/glew.h>
#include <Magick++.h>

#include <memory>
#include <string>

class Texture
{
public:
    Texture(GLenum texture_type, const std::string & filename, GLenum txr_idx);
    //Texture(const Texture&);
    void bind();
	GLuint get_idx(void) {
		return t_txr_idx;
	}

	GLuint get_obj(void) {
		return t_texture_obj;
	}

private:
    GLenum t_unit_n;
    GLenum t_txr_idx;
    GLenum t_texture_type;
    GLuint t_texture_obj;
};
