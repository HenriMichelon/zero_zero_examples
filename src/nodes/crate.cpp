module;
#include "libraries.h"

module Example;

import :Layers;
import :Crate;

Crate::Crate():
    RigidBody{
            make_shared<z0::BoxShape>(vec3{2.0f, 2.0f, 2.0f}),
            BODIES,
            WORLD | BODIES | PLAYER,
            "CrateBody"
    } {
}
