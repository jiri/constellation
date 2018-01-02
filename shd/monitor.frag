#version 330 core

uniform sampler2D image;

in vec3 vNormal;
in vec2 vUV;
in float vMaterialId;

out vec4 color;

void main() {
    if (vMaterialId == 0.0) {
        color = vec4(0.3, 0.3, 0.3, 1.0);
    }
    else {
        color = texture(image, vUV);
    }
}
