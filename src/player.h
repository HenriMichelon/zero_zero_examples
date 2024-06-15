#pragma once

class Player: public Character {
public:
    const float minMovementsSpeed = 1.5f;
    const float maxMovementsSpeed = 4.0f;
    const float acceleration = 2.0f;
    const float jumpSpeed = 5.5f;
    const float mouseSensitivity = 0.008f;
    const float viewSensitivity = 0.2f;
    const float maxCameraAngleUp = radians(60.0);
    const float maxCameraAngleDown = -radians(45.0);

    Player();

    bool onInput(InputEvent& event) override;
    void onPhysicsProcess(float delta) override;
    void onProcess(float alpha) override;
    void onReady() override;
    void onCollisionStarts(const CollisionObject::Collision collision) override;

private:
    struct State {
        vec3 velocity = VEC3ZERO;
        vec2 lookDir = VEC2ZERO;
        State& operator=(const State& other) = default;
    };

    int gamepad{-1};
    bool mouseCaptured{false};
    float mouseInvertedAxisY{1.0};
    float keyboardInvertedAxisY{1.0};
    State previousState;
    State currentState;
    float currentMovementSpeed;
    shared_ptr<Node> cameraPivot;
    shared_ptr<Camera> camera;

    void captureMouse();
    void releaseMouse();

    shared_ptr<Node> model;
    bool pushing{false};
    bool pulling{false};
    shared_ptr<ShaderMaterial> collisionOutlineMaterial;
    list<CollisionObject::Collision> previousCollisions;
};