#include <memory>
#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(
    GLFWwindow * window,
    glm::vec3 pos,
    glm::vec3 target,
    glm::vec3 up,
    float range_view,
    float horiz_angle,
    float vert_angle,
    float field_of_view,
    float move_speed,
    float mouse_speed
)   /* Init fields. */
    : c_window(window)
    , c_pos(pos)
    , c_target(target)
    , c_up(up)
    , c_range_view(range_view)
    , c_horiz_angle(horiz_angle)
    , c_vert_angle(vert_angle)
    , c_fo_view(field_of_view)
    , c_move_speed(move_speed)
    , c_mouse_speed(mouse_speed)
    , c_last_time(glfwGetTime())
{
    glfwGetWindowSize(c_window, &c_screen_width, &c_screen_height);

    glfwSetKeyCallback      (c_window, &Camera::cb_keyboard);
    glfwSetCursorPosCallback(c_window, &Camera::cb_mouse);
}

struct InternalCamera : public Camera
{
    InternalCamera(GLFWwindow * window) : Camera(window) { }
};


std::shared_ptr<Camera> Camera::get_camera(GLFWwindow * w)
{
    static std::weak_ptr<Camera> camera;
    if (auto sc = camera.lock()) {
        return sc;
    }
    auto sc = std::make_shared<InternalCamera>(w);
    camera = sc;
    return sc;
}

void Camera::cb_mouse(GLFWwindow * w, double xpos, double ypos)
{
    auto camera = get_camera();
    camera->handle_mouse(xpos, ypos);
}

void Camera::cb_keyboard(GLFWwindow * w, int key, int c, int action, int m)
{
    /* Unused. */
    (void)w; (void)c; (void)m;

    auto camera = get_camera();
    camera->handle_keyboard(key, action);
}

void Camera::handle_keyboard(int key, int action)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    switch (key) {
        case GLFW_KEY_W: {
            c_pos += (c_target * c_move_speed);
            break;
        }
        case GLFW_KEY_S: {
            c_pos -= (c_target * c_move_speed);
            break;
        }
        case GLFW_KEY_D: {
            /*
             * cross(target, up) -- cross product to create right vector.
             * normalize -- to add conistent movement (otherwise it could
             *                                         be slower or faster).
             */
            c_pos += glm::normalize(glm::cross(c_target, c_up)) * c_move_speed;
            break;
        }
        case GLFW_KEY_A: {
            c_pos -= glm::normalize(glm::cross(c_target, c_up)) * c_move_speed;
            break;
        }
    }
}

void Camera::handle_mouse(double xpos, double ypos)
{
    glfwSetCursorPos(c_window, c_screen_width / 2, c_screen_height / 2);

    /* Compute new orientation. */
    c_horiz_angle += c_mouse_speed * float( c_screen_width  / 2 - xpos);
    c_vert_angle  += c_mouse_speed * float(-c_screen_height / 2 + ypos);

    const glm::vec3 vaxis(0.0f, 1.0f, 0.0f);

    /* Rotate the view vector by the horizontal angle around
     * the vertical axis. */
    glm::vec3 view(1.0f, 0.0f, 0.0f);
    view = glm::normalize(glm::rotateY(view, c_horiz_angle));

    /* Rotate the view vector by the vertical angle around
     * the horizontal axis. */
    glm::vec3 haxis = glm::normalize(glm::cross(vaxis, view));
    view  = glm::rotate(view, c_vert_angle, haxis);

    c_target = glm::normalize(view);
    c_up     = glm::normalize(glm::cross(c_target, haxis));
}
