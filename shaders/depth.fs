#include "shadow.hpp"

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

#define GET_UNIFORM_LOC(_d, _i) \
    (_d[_i].idx = s_shader.get_uniform_location(_d[_i].name))

int Shadow::init() {
    int rc = 0;

    const char * vs_path = "./shaders/shadow.vs";
    const char * fs_path = "./shaders/shadow.fs";

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
    CHECK_RC(rc = s_shader.add_shader(GL_VERTEX_SHADER, (char *)vs));
    CHECK_RC(rc = s_shader.add_shader(GL_FRAGMENT_SHADER, (char *)fs));
    CHECK_RC(rc = s_shader.finalize());

    CHECK_UNIFORM(GET_UNIFORM_LOC(descr, 0), descr[0].name);
    //for (size_t i = 0; i < (size_t)uniform::UNIFORM_LEN; ++i) {
    //    CHECK_UNIFORM(GET_UNIFORM_LOC(descr), descr[i].name);
    //}

    free(vs);
    free(fs);
    return 0;
}

void Shadow::set_wvp(const glm::mat4 & wvp) {
    glUniformMatrix4fv(descr[WVP].idx, 1, GL_FALSE, &wvp[0][0]);
}

void Shadow::set_texture_unit(GLuint txr_idx) {
    glUniform1i(descr[SHADOW_MAP].idx, txr_idx);
}

void Shadow::enable() {
    s_shader.enable();
}

void Shadow::add_object(Object *obj, Transform *t) {
    struct objs_prop op = {obj, t};
    objs.push_back(op);
}

void Shadow::set_light(const glm::vec3 & pos,
                                const glm::vec3 & tar,
                                const glm::vec3 & up) {
    l_pos = pos;
    l_tar = tar;
    l_up = up;
}

void Shadow::render() {
    s_shader.enable();

    for (auto & obj : objs) {
        obj.t->set_camera(l_pos, l_tar, l_up);
        set_wvp(obj.t->get_wvp());
        obj.obj->render();
    }
}
