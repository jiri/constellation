#version 330 core

uniform mat4 camera;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in int materialId;

out vec3 vNormal;
out vec2 vUV;
out float vMaterialId;

void main() {
   gl_Position = camera * vec4(pos, 1.0);
   vNormal = normal;
   vUV = uv;
   vMaterialId = materialId;
}
