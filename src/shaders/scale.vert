#version 450

#include "vertex.glsl"

void main() {
    // scale the model using the parameter sent by the scene
    vec3 scaledPosition = position * vec3(1.0f + material.parameters[1].x);
    vertexParameters(scaledPosition);
}
