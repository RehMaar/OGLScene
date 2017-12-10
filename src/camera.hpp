#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

/*
 * Camera operator class
 *
 * Calculates camera position, perspective etc.
 * handles peripherial devices
 */

class Camera
{
protected:
    Camera(GLFWwindow * window,
           glm::vec3 pos       = glm:: vec3(41.062508, 52.905888, -39.517471),//vec3(30, 59.967182, 0),//vec3(0.0f, 70.0f, 0.0f),
           glm::vec3 target    = glm:: vec3(-0.603759, -0.581036, 0.545777),//vec3(-0.767405, -0.609159, 0.200037),//vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up        = glm:: vec3(-0.431030, 0.813878, 0.389636),//vec3(-0.589462, 0.793048, 0.153653),//vec3(0.0f, 0.0f, 0.0f),
           float range_view    = 500.0f,
           float horiz_angle   = M_PI,
           float vert_angle    = 0.0f,
           float field_of_view = 60.0f,
           float move_speed    = 1.0f,
           float mouse_speed   = 0.005f
    );

public:
    static std::shared_ptr<Camera> get_camera(GLFWwindow * w = nullptr);

	float get_fov(void) {
		return c_fo_view;
	}

    const glm::vec3 & get_position()
    {
        return c_pos;
    }
    const glm::vec3 & get_target()
    {
        return c_target;
    }
    const glm::vec3 & get_up()
    {
        return c_up;
    }



private:

    static void cb_keyboard(GLFWwindow * w, int key, int code, int action,
                                int mods);
    static void cb_mouse(GLFWwindow * w, double xpos, double ypos);
    void handle_keyboard(int key, int action);
    void handle_mouse(double xpos, double ypos);

    GLFWwindow * c_window;

    glm::vec3 c_pos;	/* Camera position. */
    glm::vec3 c_target;	/* Camera direction. */
    glm::vec3 c_up;		/* Vertical vector. */

    int c_screen_width;
    int c_screen_height;

    float c_horiz_angle;  /* Radians. */
    float c_vert_angle;   /* Radians. */
    float c_fo_view;      /* Degrees. */
    float c_range_view;

    float c_move_speed;   /* Units per press. */
    float c_mouse_speed;

    double c_last_time;
};
