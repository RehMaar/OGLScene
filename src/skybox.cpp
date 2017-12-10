#include <stdlib.h>
#include <Magick++.h>
#include <vector>
#include <string>
#include "skybox.hpp"

#define CHECK_RC(_expr) do {                                           \
    if ((_expr) != 0) {                                                \
        fprintf(stderr, "error: %s has %d return code.\n", #_expr, rc);\
        return -1;                                                     \
    }                                                                  \
} while(0);

#define CHECK_UNIFORM(_expr, _loc) do {                                      \
    if ((_expr) == INVALID_UNIFORM) {                                        \
        fprintf(stderr, "error: cannot get uniform location of %s.\n", _loc);\
        return -1;                                                           \
    }                                                                        \
} while (0);

#define GET_UNIFORM_LOC(_d) \
    (_d[i].idx = s_shader.get_uniform_location(_d[i].name))




int Skybox::init(GLenum txr_idx)
{
    s_txr_unit = txr_idx;
    if (init_shaders() != 0) return -1;
    if (init_texture() != 0) return -1;
    if (init_model() != 0) return -1;
    return 0;
}

int Skybox::init_texture() {
    glGenTextures(1, &s_txr_obj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s_txr_obj);

    //int width, height, nrChannels;
    std::vector<std::string> skybox_faces;
    skybox_faces.push_back("textures/orbital/orbital-element_rt.tga"); //nebula/purplenebula_rt.tga"); //"textures/skybox/right.jpg");
    skybox_faces.push_back("textures/orbital/orbital-element_lf.tga"); //nebula/purplenebula_lf.tga"); //"textures/skybox/left.jpg");
    skybox_faces.push_back("textures/orbital/orbital-element_up.tga"); //nebula/purplenebula_up.tga"); //"textures/skybox/top.jpg");
    skybox_faces.push_back("textures/orbital/orbital-element_dn.tga"); //nebula/purplenebula_dn.tga"); //"textures/skybox/bottom.jpg");
    skybox_faces.push_back("textures/orbital/orbital-element_bk.tga"); //nebula/purplenebula_bk.tga"); //"textures/skybox/back.jpg");
    skybox_faces.push_back("textures/orbital/orbital-element_ft.tga"); //nebula/purplenebula_ft.tga"); //"textures/skybox/front.jpg");
    
    for(GLuint i = 0; i < skybox_faces.size(); i++)
    {
        auto image = Magick::Image(skybox_faces[i]);
        Magick::Blob data;
        image.write(&data, "RGB");
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGB,
                     image.columns(),
                     image.rows(),
                     0, GL_RGB, GL_UNSIGNED_BYTE,
                     data.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return 0;
}

int Skybox::init_shaders() {
    int rc = 0;

    const char * vs_path = "./shaders/skybox.vs";
    const char * fs_path = "./shaders/skybox.fs";

    char * vs = Shader::load_file(vs_path);
    if (vs == NULL) {
        fprintf(stderr, "error: cannot read shader `%s'.", vs_path);
        return -1;
    }
    char * fs = Shader::load_file(fs_path);
    if (fs == NULL) {
        fprintf(stderr, "error: cannot read shader `%s'.", fs_path);
        return -1;
    } 

    CHECK_RC(rc = s_shader.init());
    CHECK_RC(rc = s_shader.add_shader(GL_VERTEX_SHADER, vs));
    CHECK_RC(rc = s_shader.add_shader(GL_FRAGMENT_SHADER, fs));
    CHECK_RC(rc = s_shader.finalize());

    for (size_t i = 0; i < (size_t)uniform::UNIFORM_LEN; ++i) {
        CHECK_UNIFORM(GET_UNIFORM_LOC(descr), descr[i].name);
    }

    free(vs);
    free(fs);
    return 0;
}

int Skybox::init_model() {

    glGenVertexArrays(1, &s_vao);
    glBindVertexArray(s_vao);
    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &s_vertices,
                 GL_STATIC_DRAW);

    return 0;
}

void Skybox::set_wvp(const glm::mat4 & wvp) {
    glUniformMatrix4fv(descr[WVP].idx, 1, GL_FALSE, &wvp[0][0]);
}

void Skybox::set_texture_unit() {
    glUniform1i(descr[SAMPLER].idx, s_txr_unit);
}

void Skybox::activate_textures() {
    glActiveTexture(GL_TEXTURE0 + s_txr_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s_txr_obj);
}

void Skybox::deactivate_textures() {
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::enable() {
    s_shader.enable();
}

void Skybox::disable() {
    s_shader.disable();
}

void Skybox::render() {
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBindVertexArray(s_vao);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(GLfloat),
            (GLvoid*)0
    );
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
}
