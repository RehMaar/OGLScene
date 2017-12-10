#version 330 core

layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 wvp;

void main()
{
    vec4 pos =  wvp * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}  
