#version 450

#include "fragment.glsl"

void main() {
    // change the gradient green color using the parameter sent by the scene
    Material material = materials.material[pushConstants.materialIndex];
    COLOR = fragmentColor(vec4(fs_in.UV.x, fs_in.UV.y, material.parameters[0].r, 0.75), true);
}
