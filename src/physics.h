#pragma once

class PhysicsMainScene: public Node, public GEventHandler {
public:
    PhysicsMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onProcess(float alpha) override;
    void onEnterScene() override;
    void onExitScene() override;
private:
    const vec3  force = vec3{10000.0f};

    shared_ptr<GWindow> menu;
    shared_ptr<Player> player;

    shared_ptr<RayCast> raycast;
    shared_ptr<ShaderMaterial> raycastOutlineMaterial;
    MeshInstance* previousSelection{nullptr};
    
    shared_ptr<GWindow> infoBox;
    shared_ptr<GText> infoText;
    shared_ptr<GText> actionsText;
    shared_ptr<ShaderMaterial> collisionOutlineMaterial;
    list<CollisionObject::Collision> currentCollisions;

    bool pushing{false};
    bool pulling{false};
    void onPushOrPull(Player::PushOrPullAction* action);
};
