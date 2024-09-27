module;
#include "libraries.h"

export module Example:Crate;
import :Layers;

// a crate with a colliding body
export class Crate : public z0::RigidBody {
public:
    Crate(): RigidBody{
        make_shared<z0::BoxShape>(vec3{2.0f, 2.0f, 2.0f}),
        BODIES,
        WORLD | BODIES | PLAYER,
        "Crate"
    } {
    }
};
