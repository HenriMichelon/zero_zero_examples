#pragma once

class Crate: public RigidBody {
public:
    Crate(): RigidBody{
        make_shared<BoxShape>(vec3{2.0f,2.0f, 2.0f}),
        Layers::BODIES,
        Layers::WORLD | Layers::BODIES | Layers::PLAYER,
        "Crate" } {}
};

Z0_REGISTER_TYPE(Crate)