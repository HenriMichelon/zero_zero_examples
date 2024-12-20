module;
#include "libraries.h"

module example.Crate;

import example.Layers;

Crate::Crate(const shared_ptr<Node>& body):
    RigidBody{
        make_shared<z0::ConvexHullShape>(body),
            BODIES,
            "CrateBody"
    } {
}

Crate::Crate():
    RigidBody{
            make_shared<z0::BoxShape>(vec3{2.0f, 2.0f, 2.0f}),
            BODIES,
            "CrateBody"
    } {
}
