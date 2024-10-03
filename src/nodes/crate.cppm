module;
#include "libraries.h"

export module Example:Crate;

// a crate with a colliding body
export class Crate : public z0::RigidBody {
public:
    Crate();
};
