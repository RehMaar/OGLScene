#include <stdio.h>
#include "object.hpp"
#include "objloader.hpp"

Object::Object(const std::string & object_filename)
    : o_object_filename(object_filename)
{
}

Object::~Object()
{
    glDeleteBuffers(1, &o_vertexbuffer);
    glDeleteBuffers(1, &o_uvbuffer);
    glDeleteBuffers(1, &o_normalbuffer);

}

int Object::init()
{

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    int res = loadOBJ(o_object_filename.c_str(), o_vertices, o_uvs, o_normals);
    if (res < 0) {
        fprintf(stderr, "error: couldn't load object.");
        return res;
    }

    glGenBuffers(1, &o_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, o_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, o_vertices.size() * sizeof(glm::vec3), &o_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &o_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, o_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, o_uvs.size() * sizeof(glm::vec2), &o_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &o_normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, o_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, o_normals.size() * sizeof(glm::vec3), &o_normals[0], GL_STATIC_DRAW);

    return 0;
}

void Object::enable_buffer(int idx, GLuint obj, size_t size) {
    glEnableVertexAttribArray(idx);
    glBindBuffer(GL_ARRAY_BUFFER, obj);
    glVertexAttribPointer(
            idx,      /* attribute */
            size,     /* size */
            GL_FLOAT, /* type */
            GL_FALSE, /* normalized? */
            0,        /* stride */
            (void *)0 /* array buffer offset */
    );
}

void Object::render()
{
    enable_buffer(0, o_vertexbuffer, 3);
    enable_buffer(1, o_uvbuffer, 2);
    enable_buffer(2, o_normalbuffer, 3);

    glDrawArrays(GL_TRIANGLES, 0, o_vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
