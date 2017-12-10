#include <stdio.h>
#include "light.hpp"

#define CHECK_RC(_expr) do {                                           \
    if ((_expr) != 0) {                                                \
        fprintf(stderr, "error: %s has %d return code.\n", #_expr, rc);\
        return -1;                                                     \
    }                                                                  \
} while(0);

#define CHECK_UNIFORM(_expr, _loc) do {                                      \
    if ((_expr) == INVALID_UNIFORM) {                                        \
        fprintf(stderr, "error: cannot get uniform location of %s.\n", _loc);\
        return -1;\
    }                                                                        \
} while (0);

#define GET_UNIFORM_LOC(_d) \
    (_d[i].idx = l_shader.get_uniform_location(_d[i].name))

int Light::init() {
    int rc = 0;

    const char * vs_path = "./shaders/light.vs";
    const char * fs_path = "./shaders/light.fs";

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

    CHECK_RC(rc = l_shader.init());
    CHECK_RC(rc = l_shader.add_shader(GL_VERTEX_SHADER, (char *)vs));
    CHECK_RC(rc = l_shader.add_shader(GL_FRAGMENT_SHADER, (char *)fs));
    CHECK_RC(rc = l_shader.finalize());

    for (size_t i = 0; i < (size_t)uniform::UNIFORM_LEN; ++i) {
        GET_UNIFORM_LOC(descr);
//        CHECK_UNIFORM(GET_UNIFORM_LOC(descr), descr[i].name);
    }

    free(vs);
    free(fs);
    return 0;
}

void Light::set_wvp(const glm::mat4 & wvp) {
    glUniformMatrix4fv(descr[WVP].idx, 1, GL_FALSE, &wvp[0][0]);
}

void Light::set_world(const glm::mat4 & world) {
    glUniformMatrix4fv(descr[WORLD].idx, 1, GL_FALSE, &world[0][0]);
}

void Light::set_texture_unit(GLuint txr_idx) {
    glUniform1i(descr[SAMPLER].idx, txr_idx);
}

void Light::set_view_point(const glm::vec3 & v) {
    glUniform3f(descr[VIEW_POS].idx, v.x, v.y, v.z);
}

void Light::set_material_reflect_i(float i) {
    glUniform1f(descr[MAT_REF_I].idx, i);
}

void Light::set_specular_power(float p) {
    glUniform1f(descr[SPEC_POWER].idx, p);
}

void Light::set_direct_light(const direct_light & l) {
    glUniform1f(descr[DL_AI].idx, l.ai);
    glUniform1f(descr[DL_DI].idx, l.di);
    glUniform3f(descr[DL_COLOR].idx, l.color.x, l.color.y, l.color.z);
    glUniform3f(descr[DL_DIRECT].idx, l.direct.x, l.direct.y, l.direct.z);
}

void Light::set_spot_light(const spot_light & l, uint64_t n) {

    glUniform3f(descr[SL_DIRECT1 + n].idx, l.direct.x, l.direct.y, l.direct.z);
    glUniform1f(descr[SL_CUTOFF1 + n].idx, cos(glm::radians(l.cutoff)));

    glUniform1f(descr[SL_PL_AI1 + n].idx, l.pl.ai);
    glUniform1f(descr[SL_PL_DI1 + n].idx, l.pl.di);
    glUniform3f(descr[SL_PL_COLOR1 + n].idx, l.pl.color.x, l.pl.color.y, l.pl.color.z);
    glUniform3f(descr[SL_PL_POS1 + n].idx, l.pl.pos.x, l.pl.pos.y, l.pl.pos.z);
    glUniform1f(descr[SL_PL_CONST1 + n].idx, l.pl.fading.f_const);
    glUniform1f(descr[SL_PL_LINEAR1 + n].idx, l.pl.fading.f_linear);
    glUniform1f(descr[SL_PL_EXP1 + n].idx, l.pl.fading.f_exp);
}

void Light::set_shadow_map(GLuint idx, uint64_t n) {
    glUniform1i(descr[SHADOW_MAP1 + n].idx, idx);
}

void Light::set_light_wvp(const glm::mat4 & light_wvp, uint64_t n) {
    glUniformMatrix4fv(descr[LIGHT_WVP1 + n].idx, 1, GL_FALSE, &light_wvp[0][0]);
}

void Light::set_transparency(float t) {
    glUniform1f(descr[TRANS].idx, t);
}
void Light::set_transparency_map(GLuint idx, uint64_t n) {
    glUniform1i(descr[TRANS_MAP1 + n].idx, idx);
}

void Light::enable(void) {
    l_shader.enable();
}
#undef CHECK_RC
