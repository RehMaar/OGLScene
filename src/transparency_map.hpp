#pragma once

#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "object.hpp"
#include "transform.hpp"

#include "shaders.hpp"

class TransparencyMap {
    public:
        TransparencyMap();
        ~TransparencyMap() = default;

        void add_object(Object *obj, Transform *t, float transparency);
        void render();
        void set_light(const glm::vec3 & pos,
                       const glm::vec3 & tar,
                       const glm::vec3 & up);
    private:

        struct object_props {
            Object    *obj;
            Transform *transform;
            float      transparency;
        };

        static bool sort_by_transparency(const object_props &a, const object_props &b) {
            return a.transparency < b.transparency;
        }

        Shader shader;
        std::vector<object_props> objs;
        glm::vec3 l_pos;
        glm::vec3 l_tar;
        glm::vec3 l_up;

        enum uniform {
            COLOR,
            WVP,
            UNIFORM_LEN
        };

        struct uniform_descr {
            GLuint idx;
            const char * name;
        } descr[UNIFORM_LEN] = {
            {INVALID_UNIFORM, "color"},
            {INVALID_UNIFORM, "wvp"},
        };
};
