#pragma once

namespace z0 {

    class RayCast : public Node {
    public:
        RayCast(const string& name);

    private:
        vec3 target{0.0};
        unique_ptr<JPH::BroadPhase> broadPhase;

    public:
        void _physicsUpdate() override;
    };

}
