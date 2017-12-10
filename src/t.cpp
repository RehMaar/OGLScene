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

#include "shaders.hpp"
#include "object.hpp"


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

int main(void) {

    GLFWwindow* window = nullptr;
    /* Initialise window. */
    gl_init(&window);
    /* Set background color */
    glClearColor(0.14, 0.22, 0.11, 0.0);


    Shader sh;
    sh.init();

    char * vs = Shader::load_file("./shaders/transp.vs");
    char * fs = Shader::load_file("./shaders/transp.fs");

    
    sh.add_shader(GL_VERTEX_SHADER, vs);
    sh.add_shader(GL_FRAGMENT_SHADER, fs);
    sh.finalize();

    free(vs);
    free(fs);

    static const GLfloat vert[9] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        0.0, 1.0, 0.0
    };

    Object obj("models/cube/test_model.obj");
    assert(obj.init() == 0);

    do {
        glClear(GL_COLOR_BUFFER_BIT);
        sh.enable();
        obj.render();
      
        glfwSwapBuffers(window);
        glfwPollEvents();
    }  while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);

    /* Close OpenGL window and terminate GLFW */
    glfwTerminate();
    return 0;
}
