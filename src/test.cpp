#include <vector>
#include <stdio.h>
#include <string>
#include <memory>
#include <sys/stat.h>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glut.h>

#include "shadow.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "texture.hpp"
#include "object.hpp"
#include "objloader.hpp"
#include "camera.hpp"
#include "shadow_map_omni.hpp"
#include "skybox.hpp"

#define TRY(eq, msg) \
    if (eq) {\
        fprintf(stderr, "error: %s", msg);\
        glfwTerminate();\
        exit(1);\
    }


using namespace glm;

static size_t height = 768;
static size_t width  = 1366;

void set_input_mode(GLFWwindow *w) {
    glfwSetInputMode(w, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(w, GLFW_CURSOR,      GLFW_CURSOR_DISABLED);
}

void set_window_hint(int samples, int major, int minor, int profile) {
    glfwWindowHint(GLFW_SAMPLES, samples);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
}

void gl_init(GLFWwindow **w) {
    // Initialise GLFW
    TRY(!glfwInit(), "Failed to initialize GLFW\n");

    set_window_hint(4, 3, 3, GLFW_OPENGL_CORE_PROFILE);

    /* Open a window and create its OpenGL context. */
    *w = glfwCreateWindow(width, height, "Test", NULL, NULL);
    TRY(w == nullptr, "Failed to open GLFW window.")

    glfwMakeContextCurrent(*w);

    glewExperimental = true;
    TRY(glewInit() != GLEW_OK, "Failed to initialize GLEW\n");

    set_input_mode(*w);

    glfwPollEvents();
    glfwSetCursorPos(*w, width / 2, height / 2);
}

void set_spot_light(spot_light & sl, vec3 & pos, vec3 tar) {
    sl.pl.ai = 2.0f;
    sl.pl.di = 12.0f;
    sl.pl.fading.f_linear = 0.1f;
    sl.pl.color = vec3(1, 1, 1);
    sl.cutoff = 95.0f;
    sl.pl.pos = pos;
    sl.direct = tar;
}

#define SET_LIGHT(_light, _ref, _sp, _dl, _sl)\
        _light.set_material_reflect_i(_ref);\
        _light.set_specular_power(_sp);    \
        _light.set_direct_light(_dl);       \
        _light.set_spot_light(_sl);


struct CameraDirection
{
    GLenum face;
    vec3 tar;
    vec3 up;
};
 
CameraDirection camera_direct[6] =
{
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, vec3(1.0f, 0.0f, 0.0f),   vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, vec3(-1.0f, 0.0f, 0.0f),  vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, vec3(0.0f, 1.0f, 0.0f),   vec3(0.0f, 0.0f, -1.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, vec3(0.0f, -1.0f, 0.0f),  vec3(0.0f, 0.0f, 1.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, vec3(0.0f, 0.0f, 1.0f),   vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, vec3(0.0f, 0.0f, -1.0f),  vec3(0.0f, -1.0f, 0.0f) }
};


int main(int argc, char *argv[]) {


    GLFWwindow* window = nullptr;
    /* Initialise window. */
    gl_init(&window);
    /* Enadble depth test. TODO: what is it?*/
    glEnable(GL_DEPTH_TEST);
    /* Set background color */
    glClearColor(0.14, 0.22, 0.11, 0.0);

    GLuint txr_ctr = 0;
// pos(-21.821548, 56.304161, 1.480841)
// tar(0.267223, -0.874789, -0.404148)
// up (0.482481, 0.484503, -0.729705)
    vec3 light_pos = vec3(-21.821548, 56.304161, 1.480841); //(-3.611092, 66.789169, 26.771217);
    vec3 light_tar = vec3(0.267223, -0.874789, -0.404148); //(-0.563166, -0.576959, -0.591576); 
    vec3 light_up  = vec3(0.482481, 0.484503, -0.729705); //(-0.397814, 0.816773, -0.417883);

    /* Init light shaders. */
    Light light;
    assert(light.init() == 0);
    spot_light sl;
    set_spot_light(sl, light_pos, light_tar);

    direct_light dl;
    dl.color = glm::vec3(1, 1, 1);
    dl.ai = 0.3f;


#define GEN_OBJECT(_name, _txr_path, _obj_path, _txr_ctr, _x, _y, _z, _tr) \
    Texture _name##_txr(GL_TEXTURE_2D, _txr_path, _txr_ctr);          \
    Object _name(_obj_path);                                          \
    assert(_name.init() == 0);                                        \
    Transform _name##_trans;                                          \
    _name##_trans.world_position(_x, _y, _z);                         \
    _name##_trans.set_perspective(60.0, width, height, 1.0, 500.0);   \
    float _name##_transparency = _tr;

    GEN_OBJECT(
        tower,
        "textures/gray.jpg", //"models/watertower/watertower.jpg",
        "models/watertower/watertower.obj",
         txr_ctr++,
         -40, 0, 20,
         1.0
    );
    tower_trans.scale(5, 5, 5);

    GEN_OBJECT(
        cube,
        //"textures/marble.jpg",
        "models/floor/floor.png",
        "models/floor/floor.obj",
         txr_ctr++,
         0, 0, 0,
         1.0
    );
    cube_trans.scale(45, 45, 45);

    ShadowMapFBOOmni shadow_fbo;
    assert(shadow_fbo.init(width, height) == 0);
    GLuint shadow_txr = txr_ctr++;

    Shadow shadow;
    assert(shadow.init() == 0);
    shadow.add_object(&tower, &tower_trans);

    auto camera = Camera::get_camera(window);
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* Current camera props. */
        auto pos = camera->get_position();
        auto tar = camera->get_target();
        auto up  = camera->get_up();
      printf("pos(%f, %f, %f) tar(%f, %f, %f) up(%f, %f, %f)\n",
             pos.x, pos.y, pos.z,
             tar.x, tar.y, tar.z,
             up.x, up.y, up.z
      );

        glCullFace(GL_FRONT);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        for (uint64_t i = 0; i < 6; i++) {
            shadow_fbo.bind_fbo(camera_direct[i].face);
            glClear(GL_DEPTH_ATTACHMENT | GL_COLOR_BUFFER_BIT);
                shadow.set_light(light_pos, camera_direct[i].tar, camera_direct[i].up);
                shadow.render();
            shadow_fbo.unbind_fbo();
        }

        /* Accept fragment that closer to the camera. */
        glDepthFunc(GL_LESS);
        /* Cull triangles which normal is not towards the camera */
        glCullFace(GL_BACK);

        //:w
        //shadow_fbo.activate_shadow_map(GL_TEXTURE0 + shadow_txr);

        light.enable();
        light.set_material_reflect_i(10);
        light.set_specular_power(10);
        light.set_direct_light(dl);
        light.set_spot_light(sl, 0);
        //light.set_shadow_map(shadow_txr, 0);
        light.set_view_point(pos);

#define RENDER(_obj)                                            \
   do {                                                         \
        _obj##_trans.set_camera(pos, tar, up);                  \
        light.set_wvp(_obj##_trans.get_wvp());                  \
        light.set_world(_obj##_trans.get_world());              \
        _obj##_trans.set_camera(light_pos, light_tar, light_up);\
        light.set_light_wvp(_obj##_trans.get_wvp(), 0);            \
        light.set_texture_unit(_obj##_txr.get_idx());           \
        light.set_transparency(_obj##_transparency);            \
        _obj##_txr.bind();                                      \
        _obj.render();                                          \
    } while(0);

        RENDER(tower);
        RENDER(cube);

        /* Swap buffers. TODO: why? */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }  while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);

    /* Close OpenGL window and terminate GLFW */
    glfwTerminate();
    return 0;
}
