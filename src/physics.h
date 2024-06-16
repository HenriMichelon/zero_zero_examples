#pragma once

class PhysicsMainScene: public Node, public GEventHandler {
public:
    PhysicsMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onProcess(float alpha) override;
    void onEnterScene() override;
    void onExitScene() override;
private:
    shared_ptr<RayCast> raycast;
    shared_ptr<ShaderMaterial> raycastOutlineMaterial;
    MeshInstance* previousSelection{nullptr};
    shared_ptr<GWindow> menu;
};
