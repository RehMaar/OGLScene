#include "transform.hpp"

/*
 * Compute a world matrix.
 */
const glm::mat4& Transform::get_world()
{
    t_world = t_world_matrix * t_rotate_matrix * t_scale_matrix;
    return t_world;
}

/*
 * Compute a World-View-Projection matrix.
 */
const glm::mat4& Transform::get_wvp()
{
    const auto & world = get_world();
    t_wvp = t_perspective_matrix * t_camera * world;
    return t_wvp;
}
