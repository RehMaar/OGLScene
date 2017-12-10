#pragma once

//#include "LightningEffect.h"
#include "texture.hpp"

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <GL/glut.h>

class Object
{
public:
    Object(const std::string & object_filename);
    ~Object();

    int init();
    void render();

private:
	void enable_buffer(int idx, GLuint obj_id, size_t size);

    std::string o_object_filename;

	GLuint vao;

    GLuint o_vertexbuffer;
    GLuint o_uvbuffer;
    GLuint o_normalbuffer;

    std::vector<glm::vec3> o_vertices;
    std::vector<glm::vec2> o_uvs;
    std::vector<glm::vec3> o_normals;
};
