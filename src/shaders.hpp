#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <GL/glew.h>
#include <vector>

#define INVALID_UNIFORM ((GLuint)0xFFFFFFFF)

class Shader
{
public:
    Shader() = default;
    ~Shader();
    int init();
    void enable();
    void disable();

    int add_shader(GLenum shader_type, const char * shader_code);
    int finalize();
    GLint get_uniform_location(const char * uniform_name);

private:
    static size_t get_file_size(FILE *f) {
        struct stat st;

        int fd = fileno(f);
        fstat(fd, &st);
        return st.st_size;
    }

public:
    static char * load_file(const char * filename) {
        FILE *f = fopen(filename, "rb");
        if (f == NULL) {
            fprintf(stderr, "error: cannot open %s.\n", filename);
            exit(-1);
        }
        size_t len = get_file_size(f);
        char * buf = (char *)malloc(sizeof(char)*(len));
        ssize_t r = fread(buf, len, 1, f);
        if (r < 0) {
            perror("fread");
            exit(-1);
        }
        buf[len - 1] = 0;
        return buf;
    }
private:
    GLuint s_shader_prog;

    using ShaderObjectList = std::vector<GLuint>;
    ShaderObjectList s_shader_obj_list;
};
