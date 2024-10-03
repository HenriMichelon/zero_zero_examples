module;
#include "libraries.h"

export module Example:Player;

import :Layers;

using namespace z0;

// The main player class
export class Player : public Character {
public:
    // signal emmited on user interaction
    static const Signal::signal on_push_pull;

    // parameters for the above signal
    struct PushOrPullAction : Signal::Parameters {
        bool push; // player want to push a crate
        bool pull; // player want to pull a crate
    };

    Player();

    bool onInput(InputEvent &event) override;

    void onPhysicsProcess(const float delta) override;

    void onProcess(const float alpha) override;

    void onReady() override;

    // release the mouse to access the menus
    void releaseMouse();

protected:
    bool cameraCollisions{true};

private:
    // movement and view state
    struct State {
        vec3 velocity{VEC3ZERO};
        vec2 lookDir{VEC2ZERO};

        State &operator=(const State &other) = default;
    };

    State previousState;
    State currentState;

    // movement starting speed
    const float minMovementsSpeed{1.5f};
    // maximum movement speed
    const float maxMovementsSpeed{4.0f};
    // acceleration from minMovementsSpeed to maxMovementsSpeed
    const float acceleration{2.0f};
    // jump height
    const float jumpSpeed{5.5f};
    // view rotation speed when the mouse is used
    const float mouseSensitivity{0.007f};
    // view rotaion speed when the gamepad or keyboard is used
    const float viewSensitivity{0.2f};
    // max camera vertical angle
    const float maxCameraAngleUp{radians(60.0)};
    // min camera vertical angle
    const float maxCameraAngleDown{-radians(45.0)};

    // current game pad id
    int gamepad{-1};
    // did the mouse is captured ?
    bool mouseCaptured{false};
    // invert the vertical mouse axis
    float mouseInvertedAxisY{1.0};
    // invert the vertical keyboard axis
    float keyboardInvertedAxisY{1.0};
    // current player speed
    float currentMovementSpeed{minMovementsSpeed};
    // player model
    shared_ptr<Node> model;
    // player camera
    shared_ptr<Camera> camera;
    // pivot to rotate the camera
    shared_ptr<Node> cameraPivot;
    // camera pivot attachement point
    shared_ptr<Node> cameraAttachement;
    // camera view collision sensor
    shared_ptr<CollisionArea> cameraCollisionSensor;
    // object colliding with the camera view
    CollisionObject *cameraCollisionTarget{nullptr};
    // counter used to avoid too much camera movements in case of a collision
    int cameraCollisionCounter{0};
    // starting value of the above counter
    int cameraCollisionCounterMax{100};
    // tween used for a smooth camera movement in case of a camera view collision
    shared_ptr<Tween> cameraInTween;
    // tween used for a smooth camera movement when the camera returns to the original position
    shared_ptr<Tween> cameraOutTween;

    // capture the mouse to allow the use of the keyboard & gamepad
    void captureMouse();

    // signal handler called when the camera collide or stop collinding with something
    void onCameraCollision(const Collision *collision);
};
