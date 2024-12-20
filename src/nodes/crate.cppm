module;
#include "libraries.h"

export module example.Crate;

// a crate with a colliding body
export class Crate : public z0::RigidBody {
public:
    Crate();
    Crate(const shared_ptr<Node>& body);
};
