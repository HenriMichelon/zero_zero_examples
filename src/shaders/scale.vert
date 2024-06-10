#version 450

#include "vertex.glsl"

void main() {
    vec3 scaledPosition = position * vec3(1.0f + material.parameters[1].x);
    vertexParameters(scaledPosition);
}
