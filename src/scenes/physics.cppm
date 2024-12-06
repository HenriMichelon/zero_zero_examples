module;
#include <simdjson.h>
#include "libraries.h"

export module Example:PhysicsMainScene;

import :Player;
import :Crate;
import :Layers;

export class PhysicsMainScene : public Node {
public:
    PhysicsMainScene();

    void onReady() override;

    void onProcess(float alpha) override;

    void onEnterScene() override;

    void onExitScene() override;

private:
    // force used when pulling or pushing a crate
    const vec3 force = vec3{7500.0f};

    // scene menu
    shared_ptr<ui::Window> menu;
    // ou player node
    shared_ptr<Player> player;
    // raycast used to outline the crate in front of the player
    shared_ptr<RayCast> raycast;
    // material for the selection outline
    shared_ptr<ShaderMaterial> raycastOutlineMaterial;
    // used to save the currently selected crate to disable the outline
    shared_ptr<MeshInstance> previousSelection{nullptr};
    // information box used when the player collide with a crate
    shared_ptr<ui::Window> infoBox;
    // text displaying the colliding object name
    shared_ptr<ui::Text> infoText;
    // text displaying the actions the player can do when colliding a crate
    shared_ptr<ui::Text> actionsText;
    // outline material for colliding crates
    shared_ptr<ShaderMaterial> collisionOutlineMaterial;
    // list a colliding crate to reset the outline materials
    list<CollisionObject::Collision> currentCollisions;

    // player actions : push or pull a crate
    bool pushing{false};
    bool pulling{false};

    // signal handler called on a player action
    void onPushOrPull(Player::PushOrPullAction *action);
};
