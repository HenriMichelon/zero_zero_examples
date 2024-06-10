#pragma once

class RaycastMainScene: public Node, public GEventHandler {
public:
    RaycastMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onProcess(float alpha) override;
private:
    shared_ptr<RayCast> raycast;
    shared_ptr<ShaderMaterial> raycastOutlineMaterial;
    MeshInstance* previousSelection{nullptr};
};
