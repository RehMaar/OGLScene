#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "shaders.hpp"


Shader::~Shader() {
    for (const auto & shader_id : s_shader_obj_list) {
        glDeleteShader(shader_id);
    }
    if (s_shader_prog != 0) {
        glDeleteProgram(s_shader_prog);
    }
}

int Shader::init() {
    s_shader_prog = glCreateProgram();
    if (s_shader_prog == 0) {
        fprintf(stderr, "error: cannot create shader program\n");
        return -1;
    }
    assert(s_shader_prog != 0);
    return 0;
}

int Shader::add_shader(GLenum shader_type, const char * shader_code) {

    GLuint shader_obj = glCreateShader(shader_type);

    if (shader_obj == 0) {
        fprintf(stderr, "error: cannot create shader with type %d\n",
                        shader_type);
        return -1;
    }

    s_shader_obj_list.emplace_back(shader_obj);

    //puts(shader_code);

    GLint lengths;
    lengths = strlen(shader_code);
    glShaderSource(shader_obj, 1, (const GLchar * const *)&shader_code, &lengths);

    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar info_log[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, info_log);
        fprintf(stderr, "error: failed to compile shader type %s (%d): %s",
                        shader_type == GL_FRAGMENT_SHADER ? "FS" :
                        shader_type == GL_VERTEX_SHADER ? "VS" : "Unknown type",
                        shader_type,
                        info_log);
        return -1;
    }

    glAttachShader(s_shader_prog, shader_obj);

    return 0;
}

int Shader::finalize() {
    GLint success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(s_shader_prog);

    glGetProgramiv(s_shader_prog, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(s_shader_prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "error: failed to link shader program: '%s'\n", ErrorLog);
        return -1;
    }

    glValidateProgram(s_shader_prog);
    glGetProgramiv(s_shader_prog, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(s_shader_prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "error: invalid shader program: '%s'\n", ErrorLog);
        return -1;
    }
    glGetProgramiv(s_shader_prog, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(s_shader_prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "error: unable to link program: '%s'\n", ErrorLog);
        return -1;
    }
    GLint len;
    glGetProgramiv(s_shader_prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        glGetProgramInfoLog(s_shader_prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "shader error: '%s'\n", ErrorLog);
        return -1;
    }

    for (const auto & shader_id : s_shader_obj_list) {
        glDeleteShader(shader_id);
    }
    s_shader_obj_list.clear();

    return 0;
}

void Shader::enable() {
    glUseProgram(s_shader_prog);
}

void Shader::disable() {
    glUseProgram(0);
}

GLint Shader::get_uniform_location(const char * uniform_name) {
    GLuint location = glGetUniformLocation(s_shader_prog, uniform_name);

    if (location == 0xFFFFFFFF) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", uniform_name);
    }

    return location;
}
