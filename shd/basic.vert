#version 330 core

uniform mat4 camera;

layout (location = 0) in vec3 pos;

void main() {
   gl_Position = camera * vec4(pos, 1.0);
}
