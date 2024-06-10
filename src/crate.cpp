#include "includes.h"
#include "crate.h"
#include "layers.h"

const string Crate::MESH_PATH{"res_models_crate.glb/Sketchfab_model/Collada visual scene group/g/defaultMaterial"};

Crate::Crate(const shared_ptr<Node>& model):
    RigidBody{
        make_shared<BoxShape>(vec3{2.0f,2.0f, 2.0f}),
        Layers::BODIES,
        Layers::WORLD | Layers::BODIES | Layers::PLAYER,
        "Crate" } {
    addChild(model);
    setBounce(0.8);
    setGravityScale(0.5);
}

void Crate::onReady() {
    glm::quat rot = angleAxis(radians(static_cast<float>(rand()%90)), AXIS_Z);
    setRotation(rot);
}
