#version 330
 
layout(location = 0) in vec3 pos; 
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normals;
 
out vec2 UV; 
out vec3 normals_out;
out vec3 world;

out vec4 light_space_pos1;
out vec4 light_space_pos2;
 
uniform mat4 s_WVP; // World-View-Projection
uniform mat4 s_world;
uniform mat4 s_light_wvp1;
uniform mat4 s_light_wvp2;

void main() {
	gl_Position      = s_WVP * vec4(pos, 1.0);
	light_space_pos1 = s_light_wvp1 * vec4(pos, 1.0);
	light_space_pos2 = s_light_wvp2 * vec4(pos, 1.0);
    normals_out      = (s_world * vec4(normals, 0.0)).xyz;
    world            = (s_world * vec4(pos, 1.0)).xyz;
    UV               = vertexUV; 
}
