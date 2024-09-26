module;
#include "libraries.h"
import Z0;
using namespace z0;

export module Example:Crate;
import :Layers;

export class Crate : public RigidBody {
public:
    Crate(): RigidBody{
        make_shared<BoxShape>(vec3{2.0f, 2.0f, 2.0f}),
        Layers::BODIES,
        Layers::WORLD | Layers::BODIES | Layers::PLAYER,
        "Crate"
    } {
    }
};
