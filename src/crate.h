#pragma once

class Crate: public RigidBody {
public:
    explicit Crate(const shared_ptr<Node>& model);
    void onReady() override;
};
