#pragma once

class PhysicsMainScene: public Node {
public:
    PhysicsMainScene(): Node{"Main Scene"} {};
    void onReady() override;
};
