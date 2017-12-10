#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform.hpp>

class Transform
{
public:
    Transform()
    {
        t_scale_matrix = glm::mat4(1.0f);
        t_world_matrix = glm::mat4(0.0f);
        t_rotate_matrix = glm::mat4(1.0f);
    }

	/*
	 * Build a scale matrix.
	 * Scales each component by a scalar.
	 * | sx 0   0 |   |x|   | sx * x |
	 * | 0  sy  0 | * |y| = | sy * y |
	 * | 0  0  sz |   |z|   | sz * z |
	 */
    void scale(float x, float y, float z)
    {
        t_scale_matrix = glm::scale(glm::vec3(x, y, z));
    }

	/*
	 * Build a translation matrix. TM moves a vector
	 * a certain distance to a certain direction.
	 * | 1 0 0 X |   |x|   | X + x |
	 * | 0 1 0 Y | * |y| = | Y + y |
	 * | 0 0 1 Z |   |z|   | Z + z |
	 * | 0 0 0 1 |   |1|   |   1   |
	 */
    void world_position(float x, float y, float z)
    {
        t_world_matrix = glm::translate(glm::vec3(x, y, z));
    }

    
	/*
	 * Build a rotation matrix. 
	 */
    void rotate(float angle, const glm::vec3 & vector)
    {
        t_rotate_matrix = glm::rotate(angle, vector);
    }

 	/*
 	 * Create perspective matrix, that defines "truncated pyramid of view".
 	 */
    void set_perspective(float fov, float width, float height, float znear, float zfar)
    {
        t_perspective_matrix = glm::perspective(glm::radians(fov), width / height, znear, zfar);
    }

	/*
	 * lookAt(eye, center, up)
	 * 
	 * Build a look at view matrix.
	 */
    void set_camera(const glm::vec3 & position, const glm::vec3 & target, const glm::vec3 & up)
    {
        t_camera = glm::lookAt(position, position + target, up);
    }

    const glm::mat4 & get_wvp();

    const glm::mat4 & get_world();

private:
    glm::mat4 t_scale_matrix;
    glm::mat4 t_world_matrix;
    glm::mat4 t_rotate_matrix;
    glm::mat4 t_perspective_matrix;
    glm::mat4 t_camera;
    glm::mat4 t_wvp;
    glm::mat4 t_world;
};
