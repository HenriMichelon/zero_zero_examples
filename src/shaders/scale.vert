#version 450

#include "vertex.glsl"

void main() {
    // scale the model using the parameter sent by the scene
    Material material = materials.material[pushConstants.materialIndex];
    vec3 scaledPosition = position * vec3(1.0f + material.parameters[1].x);
    vertexParameters(scaledPosition);
}
