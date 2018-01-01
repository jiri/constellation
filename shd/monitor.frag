#version 330 core

in vec3 vNormal;
in vec2 vUV;
in float vMaterialId;

out vec4 color;

void main() {
    if (vMaterialId == 0.0) {
        color = vec4(0.3, 0.3, 0.3, 1.0);
    }
    else {
        color = vec4(vUV, 0.0, 1.0);
    }
}
