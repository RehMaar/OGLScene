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
#include "shadow_map.hpp"
#include "skybox.hpp"
#include "draw_fbo.hpp"
#include "transparency_map.hpp"

#define TRY(eq, msg) \
    if (eq) {\
        fprintf(stderr, "error: %s", msg);\
        glfwTerminate();\
        exit(1);\
    }


using namespace glm;

static size_t width  = 1366;
static size_t height = 768;

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
    sl.pl.di = 10.0f;
    sl.pl.fading.f_linear = 0.1f;
    sl.cutoff = 45.0f;
    sl.pl.pos = pos;
    sl.direct = tar;
}

int main(void) {

    GLFWwindow* window = nullptr;
    /* Initialise window. */
    glfwWindowHint(GLFW_SAMPLES, 4);
    gl_init(&window);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);  

    /* Set background color */
    glClearColor(1.0, 1.0, 1.0, 1.0);

    uint64_t txr_ctr = 0;
    auto camera = Camera::get_camera(window);

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
        "models/watertower/watertower.jpg",
        "models/watertower/watertower.obj",
         txr_ctr++,
         16, 0, 10,
         1.0
    );
    tower_trans.scale(5, 5, 5);

    GEN_OBJECT(
        rock,
        "textures/ground2.jpg",
        //"models/floor/floor.png",
        "models/floor/floor.obj",
         txr_ctr++,
         0, 0, 0,
         1.0
    );
    rock_trans.scale(15, 15, 15);

    GEN_OBJECT(
        house,
        "models/farm_house/house.jpg",
        "models/farm_house/house.obj",
        txr_ctr++,
        -20, 0, -5,
        1.0
    );

    GEN_OBJECT(
        lamp,
        "textures/not_so_red.jpg",
        "models/sphere/sphere.obj",
        txr_ctr++,
        0, 0, 0,
        1.0
    );


    GEN_OBJECT(
        lamp2,
        "textures/not_so_red.jpg",
        "models/sphere/sphere.obj",
        txr_ctr++,
        0, 0, 0,
        1.0
    );

    vec3 const_light_pos1 = vec3(41.062508, 52.905888, -39.517471);
    vec3 light_tar1= vec3(-0.603759, -0.581036, 0.545777);
    vec3 light_up1 = vec3(-0.431030, 0.813878, 0.389636);

    vec3 const_light_pos2 = vec3(-100, 49.967182, -70);
    vec3 light_tar2 = vec3(0.729543, -0.389419, 0.562246);
    vec3 light_up2 = vec3(0.308446, 0.921061, 0.237714);


    /* Init light shaders. */
    Light light;
    assert(light.init() == 0);

    spot_light sl1;
    sl1.pl.color = vec3(1, 1, 1);

    spot_light sl2;
    sl2.pl.color = vec3(1, 1, 1);

    direct_light dl;
    dl.color = glm::vec3(1, 1, 1);
    dl.ai = 0.5f;

    /* Init shadow. */
    ShadowMapFBO shadow_fbo1;
    assert(shadow_fbo1.init(width, height) == 0);
    GLuint shadow_txr1 = txr_ctr++;

    ShadowMapFBO shadow_fbo2;
    assert(shadow_fbo2.init(width, height) == 0);
    GLuint shadow_txr2 = txr_ctr++;

    Shadow shadow;
    assert(shadow.init() == 0);
    shadow.add_object(&house, &house_trans);
    shadow.add_object(&tower, &tower_trans);

    /* Init skybox */
    Skybox skybox;
    assert(skybox.init(0) == 0);
    Transform skybox_trans;
    skybox_trans.set_perspective(camera->get_fov(), width, height, 1.0, 1000.0);

    /* Init transparency FBO and map. */
    DrawFBO d_fbo(width, height);
    GLuint d_fbo_txr = txr_ctr++;

    DrawFBO d_fbo2(width, height);
    GLuint d_fbo2_txr = txr_ctr++;

    TransparencyMap t_map;
    t_map.add_object(&rock, &rock_trans, rock_transparency);
    t_map.add_object(&house, &house_trans, house_transparency);
    t_map.add_object(&tower, &tower_trans, tower_transparency);


    uint64_t i = 0;
    do {

        vec3 light_pos1;
        light_pos1.x = const_light_pos1.x;// + sin(glm::radians(float(i)))*20;
        light_pos1.y = const_light_pos1.y;
        light_pos1.z = const_light_pos1.z;// + cos(glm::radians(float(i)))*20;
        i++;

        vec3 light_pos2;
        light_pos2.x = const_light_pos2.x;// + sin(-glm::radians(float(i))/3)*10;
        light_pos2.y = const_light_pos2.y;
        light_pos2.z = const_light_pos2.z; // + cos(glm::radians(float(i))/3)*10;
        i++;

        /* Render transparency map. */
        glDepthFunc(GL_LESS);
        d_fbo.bind();
            glClearColor(0, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            t_map.set_light(light_pos1, light_tar1, light_up1);
            t_map.render();
        d_fbo.unbind();


        d_fbo2.bind();
            glClearColor(0, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            t_map.set_light(light_pos2, light_tar2, light_up2);
            t_map.render();
        d_fbo2.unbind();

        /* Render shadow map. */
        shadow_fbo1.bind_fbo();
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            shadow.set_light(light_pos1, light_tar1, light_up1);
            shadow.render();
        shadow_fbo1.unbind_fbo();

        shadow_fbo2.bind_fbo();
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            shadow.set_light(light_pos2, light_tar2, light_up2);
            shadow.render();
        shadow_fbo2.unbind_fbo();


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto pos = camera->get_position();
        auto tar = camera->get_target();
        auto up  = camera->get_up();

      printf("pos(%f, %f, %f) tar(%f, %f, %f) up(%f, %f, %f)\n",
             pos.x, pos.y, pos.z,
             tar.x, tar.y, tar.z,
             up.x, up.y, up.z
      );


        set_spot_light(sl1, light_pos1, light_tar1);
        set_spot_light(sl2, light_pos2, light_tar2);


        glCullFace(GL_BACK);
        light.enable();

        light.set_material_reflect_i(10);
        light.set_specular_power(10);
        light.set_direct_light(dl);
        light.set_spot_light(sl1, 0);
        light.set_spot_light(sl2, 1);

        light.set_view_point(pos);

        shadow_fbo1.activate_shadow_map(GL_TEXTURE0 + shadow_txr1);
        shadow_fbo2.activate_shadow_map(GL_TEXTURE0 + shadow_txr2);
        d_fbo.activate(GL_TEXTURE0 + d_fbo_txr);
        d_fbo2.activate(GL_TEXTURE0 + d_fbo2_txr);

        light.set_shadow_map(shadow_txr1, 0);
        light.set_shadow_map(shadow_txr2, 1);
        light.set_transparency_map(d_fbo_txr, 0);
        light.set_transparency_map(d_fbo2_txr, 1);

#define RENDER(_obj)                                            \
   do {                                                         \
        _obj##_trans.set_camera(pos, tar, up);                  \
        light.set_wvp(_obj##_trans.get_wvp());                  \
        light.set_world(_obj##_trans.get_world());              \
        _obj##_trans.set_camera(light_pos1, light_tar1, light_up1);\
        light.set_light_wvp(_obj##_trans.get_wvp(), 0);            \
        _obj##_trans.set_camera(light_pos2, light_tar2, light_up2);\
        light.set_light_wvp(_obj##_trans.get_wvp(), 1);            \
        light.set_texture_unit(_obj##_txr.get_idx());           \
        light.set_transparency(_obj##_transparency);            \
        _obj##_txr.bind();                                      \
        _obj.render();                                          \
    } while(0);

        lamp_trans.world_position(light_pos1.x, light_pos1.y, light_pos1.z);
        RENDER(lamp);

        lamp2_trans.world_position(light_pos2.x, light_pos2.y, light_pos2.z);
        RENDER(lamp2);

        RENDER(house);
        RENDER(tower);

        RENDER(rock);
        {
            glDepthFunc(GL_LEQUAL);
            glCullFace(GL_BACK);

            skybox.enable();
            skybox_trans.world_position(pos.x, pos.y, pos.z);
            skybox_trans.set_camera(pos, tar, up);
            skybox.set_wvp(skybox_trans.get_wvp());
            skybox.set_texture_unit();
            skybox.activate_textures();
            skybox.render();

            skybox.deactivate_textures();
            skybox.disable();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }  while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);

    /* Close OpenGL window and terminate GLFW */
    glfwTerminate();
    return 0;
}
