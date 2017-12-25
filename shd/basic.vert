#version 330 core

uniform mat4 camera;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 vNormal;

void main() {
   gl_Position = camera * vec4(pos, 1.0);
   vNormal = normal;
}
