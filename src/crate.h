#pragma once

class Crate: public RigidBody {
public:
    static const string MESH_PATH;
    explicit Crate(const shared_ptr<Node>& model);
    void onReady() override;
};
