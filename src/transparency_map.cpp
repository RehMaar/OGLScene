#include <assert.h>

#include "transparency_map.hpp"

#define CHECK_UNIFORM(_expr, _loc) do {                                      \
    if ((_expr) == INVALID_UNIFORM) {                                        \
        fprintf(stderr, "error: cannot get uniform location of %s.\n", _loc);\
        assert(false);\
    }                                                                        \
} while (0);

#define GET_UNIFORM_LOC(_d) \
    (_d[i].idx = shader.get_uniform_location(_d[i].name))

TransparencyMap::TransparencyMap() {

    const char * vs_path = "./shaders/transp.vs";
    const char * fs_path = "./shaders/transp.fs";

    char * vs = Shader::load_file(vs_path);
    if (vs == NULL) {
        fprintf(stderr, "error: cannot read shader `%s'.", vs_path);
        assert(false);
    }
    char * fs = Shader::load_file(fs_path);
    if (fs == NULL) {
        fprintf(stderr, "error: cannot read shader `%s'.", fs_path);
        assert(false);
    }

    assert(shader.init() == 0);
    assert(shader.add_shader(GL_VERTEX_SHADER, vs) == 0);
    assert(shader.add_shader(GL_FRAGMENT_SHADER, fs) == 0);
    assert(shader.finalize() == 0);

    for (size_t i = 0; i < UNIFORM_LEN; i++) {
        CHECK_UNIFORM(GET_UNIFORM_LOC(descr), descr[i].name);
    }
    free(vs);
    free(fs);
}

void TransparencyMap::add_object(Object *obj, Transform *t, float transparency) {
    struct object_props op = {obj, t, transparency};
    objs.push_back(op);
}

void TransparencyMap::render() {
    std::sort(objs.begin(), objs.end(), sort_by_transparency);

    shader.enable();
    for (auto &obj : objs) {
        obj.transform->set_camera(l_pos, l_tar, l_up);
        auto wvp = obj.transform->get_wvp();
        glUniformMatrix4fv(descr[WVP].idx, 1, GL_FALSE, glm::value_ptr(wvp));

        if (obj.transparency == 1.0) {
            glUniform4f(descr[COLOR].idx, 0.0, 0.0, 1.0, 1.0);
            obj.obj->render();
        } else {
            glDepthMask(false);
            glUniform4f(descr[COLOR].idx, 1.0, 0.0, 0.0, obj.transparency);
            obj.obj->render();
            glDepthMask(true);
        }
    }
}

void TransparencyMap::set_light(const glm::vec3 & pos,
                                const glm::vec3 & tar,
                                const glm::vec3 & up) {
    l_pos = pos;
    l_tar = tar;
    l_up = up;
}
