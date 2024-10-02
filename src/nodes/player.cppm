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

    Player():
        // the player have a physic body for the collisions
        Character{
                make_shared<BoxShape>(vec3{0.8f, 2.0f, 0.8f}),
                PLAYER,
                WORLD | BODIES
        } {
    }

    bool onInput(InputEvent &event) override {
        // if the mouse is captured the player can rotate the view with the mouse
        if (mouseCaptured && (event.getType() == INPUT_EVENT_MOUSE_MOTION)) {
            const auto &eventMouseMotion = dynamic_cast<InputEventMouseMotion &>(event);
            const auto  yRot             = -eventMouseMotion.getRelativeX() * mouseSensitivity;
            rotateY(yRot);
            cameraPivot->rotateX(eventMouseMotion.getRelativeY() * mouseSensitivity * mouseInvertedAxisY);
            cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX(), maxCameraAngleDown, maxCameraAngleUp));
            return true;
        }
        // capture the mouse on the first click if not already captured
        if ((event.getType() == INPUT_EVENT_MOUSE_BUTTON) && (!mouseCaptured)) {
            const auto &eventMouseButton = dynamic_cast<InputEventMouseButton &>(event);
            if (!eventMouseButton.isPressed()) {
                captureMouse();
                return true;
            }
        }
        // if the mouse is captured process the player keyboard events
        if ((event.getType() == INPUT_EVENT_KEY) && mouseCaptured) {
            const auto &eventKey = dynamic_cast<InputEventKey &>(event);
            // release the mouse with the ESC key
            if ((eventKey.getKey() == KEY_ESCAPE) && !eventKey.isPressed()) {
                releaseMouse();
                return true;
            }
            // check if the player want to pull a push a crate with O and P
            auto params = PushOrPullAction{
                    .push = (eventKey.getKey() == KEY_P) && eventKey.isPressed(),
                    .pull = (eventKey.getKey() == KEY_O) && eventKey.isPressed()
            };
            // send the push/pull state to the scene
            emit(on_push_pull, &params);
        }
        // if the mouse is captured process the game pad events
        if ((event.getType() == INPUT_EVENT_GAMEPAD_BUTTON) && mouseCaptured) {
            const auto &eventGamepadButton = dynamic_cast<InputEventGamepadButton &>(event);
            // release the mouse with the START button
            if ((eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_START) && !eventGamepadButton.isPressed()) {
                releaseMouse();
                return true;
            }
            // check if the player want to pull a push a crate with RL or RB
            auto params = PushOrPullAction{
                    .push = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_RB) && eventGamepadButton.
                    isPressed(),
                    .pull = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_LB) && eventGamepadButton.
                    isPressed()
            };
            // send the push/pull state to the scene
            emit(on_push_pull, &params);
        }
        // continue to propagate the event
        return false;
    }

    void onPhysicsProcess(const float delta) override {
        // In case of a collision we move the camera on top of the player head
        // to have a closer view of the colliding object
        if ((cameraCollisionTarget != nullptr) && ((cameraInTween == nullptr) || (!cameraInTween->isRunning()))) {
            const auto pos  = cameraAttachement->getPosition();
            const auto dest = vec3{0.0f, 0.0f, -pos.z};
            if (cameraPivot->getPosition() != dest) {
                // we use a tween for a smooth camera movement
                cameraInTween = cameraPivot->createPropertyTween(
                        static_cast<PropertyTween<vec3>::Setter>(&Node::setPosition),
                        cameraPivot->getPosition(),
                        dest,
                        0.5f);
                // stop a possible existing tween to avoid movements collision
                cameraPivot->killTween(cameraOutTween);
            }
        }

        // reset the movements states
        previousState = currentState;
        currentState  = State{};

        // get the input vector, first from the game pad, second for the keyboard
        vec2 input = VEC2ZERO;
        if (gamepad != -1) {
            input = Input::getGamepadVector(gamepad, GAMEPAD_AXIS_LEFT);
        }
        if (input == VEC2ZERO) {
            input = Input::getKeyboardVector(KEY_A, KEY_D, KEY_W, KEY_S);
        }

        // Determine news basic velocity
        const auto onGround                = isOnGround();
        const auto currentVerticalVelocity = dot(getVelocity(), getUpVector()) * getUpVector();
        if (onGround) {
            // we move if the player is on the ground or on an object
            currentState.velocity = getGroundVelocity();
            // jump !
            if (Input::isKeyPressed(KEY_SPACE) || Input::isGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_A)) {
                currentState.velocity += (jumpSpeed + currentMovementSpeed / 2.0f) * getUpVector();
            }
        } else {
            // preserve the vertical velocity if we are in the air
            currentState.velocity = currentVerticalVelocity;
            // we can't escape gravity
            currentState.velocity += app().getGravity() * getUpVector() * delta;
        }

        // check if the player want to move
        if (input != VEC2ZERO) {
            // capture the mouse if the player want to move
            if (!mouseCaptured) { captureMouse(); }
            if (onGround) {
                // if the player is on the ground or on an object we move in the input direction
                const auto direction = TRANSFORM_BASIS * vec3{input.x, 0, input.y};
                if (currentMovementSpeed == 0) {
                    // player start moving
                    currentMovementSpeed = minMovementsSpeed;
                } else {
                    // player already moving, accelerate
                    currentMovementSpeed += acceleration * delta;
                    currentMovementSpeed = std::min(currentMovementSpeed, maxMovementsSpeed);
                }
                // move
                currentState.velocity += direction * currentMovementSpeed;
            }
        } else {
            // stop all movements
            currentMovementSpeed = 0;
        }
        if (!onGround) {
            // Player doing a jump, preserve horizontal velocity.
            // We can't stop while in the air !
            const auto currentHorizontalVelocity = previousState.velocity - currentVerticalVelocity;
            currentState.velocity += currentHorizontalVelocity;
        }

        // If the mouse is captured we can use the game pad or the keyboard
        // to rotate the view
        if (mouseCaptured) {
            vec2 inputDir = VEC2ZERO;
            if (gamepad != -1) {
                inputDir = Input::getGamepadVector(gamepad, GAMEPAD_AXIS_RIGHT);
            }
            if (inputDir == VEC2ZERO) {
                inputDir = Input::getKeyboardVector(KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN);
            }
            if (inputDir != VEC2ZERO) {
                currentState.lookDir = inputDir * viewSensitivity * delta;
            }
        }
    }

    void onProcess(const float alpha) override {
        // If the camera is on top of the player's head
        // and there is no more camera view collision
        // we move the camera backward
        if ((cameraCollisionTarget != nullptr) && (!cameraCollisionSensor->wereInContact(cameraCollisionTarget))) {
            cameraCollisionCounter -= 1;
            if (cameraCollisionCounter == 0) {
                // we use a tween for a smooth camera movement
                cameraOutTween = cameraPivot->createPropertyTween(
                        PropertyTween<vec3>::Setter(&Node::setPosition),
                        cameraPivot->getPosition(),
                        VEC3ZERO,
                        0.5f);
                // stop a possible existing tween to avoid movements collision
                cameraPivot->killTween(cameraInTween);
                cameraCollisionTarget = nullptr;
            }
        }

        // if ((currentState.velocity != VEC3ZERO) || (!isOnGround())) {
        // move the player using an interpolation of the velocity computed during onPhysicsProcess()
        setVelocity(previousState.velocity * (1.0f - alpha) + currentState.velocity * alpha);
        // } else {
        //     // stop the player
        //     setVelocity(VEC3ZERO);
        // }
        // rotate the view
        if (currentState.lookDir != VEC2ZERO) {
            const auto interpolatedLookDir = previousState.lookDir * (1.0f - alpha) + currentState.lookDir * alpha;
            const auto yRot                = -interpolatedLookDir.x * 2.0f;
            rotateY(yRot);
            cameraPivot->rotateX(interpolatedLookDir.y * keyboardInvertedAxisY);
            cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX(), maxCameraAngleDown, maxCameraAngleUp));
        }
    }

    void onReady() override {
        // we capture the mouse at startup, ready to play !
        captureMouse();

        // create the player node
        model = Loader::loadModelFromFile("res/models/capsule.glb", true);
        model->setPosition({0.0, -1.8 / 2.0, 0.0});
        addChild(model);

        // create the attachement point for the camera pivot
        cameraAttachement       = make_shared<Node>("cameraAttachement");
        auto attachementZOffset = 2.0f;
        auto attachementYOffset = 1.6f;
        cameraAttachement->setPosition({0.0, attachementYOffset, attachementZOffset});
        addChild(cameraAttachement);

        // create the collision sensor used to detect if the camera view area collide with something
        if (cameraCollisions) {
            cameraCollisionSensor = make_shared<CollisionArea>(
                    make_shared<SphereShape>(attachementZOffset - 0.6f),
                    WORLD | BODIES,
                    "cameraCollisionNode"
                    );
            cameraCollisionSensor->setPosition({0.0, attachementYOffset, attachementZOffset / 2.0f - 0.5f});
            cameraCollisionSensor->connect(on_collision_starts,
                                           this,
                                           Signal::Handler(&Player::onCameraCollision));
            cameraCollisionSensor->connect(on_collision_persists,
                                           this,
                                           Signal::Handler(&Player::onCameraCollision));
            addChild(cameraCollisionSensor);
        }

        // create the pivot used to rotate the camera
        cameraPivot = make_shared<Node>("cameraPivot");
        cameraAttachement->addChild(cameraPivot);

        // create the player camera and attach it to the pivot
        camera = make_shared<Camera>("Player camera");
        camera->setPerspectiveProjection(75.0, 0.1, 200.0);
        cameraPivot->addChild(camera);
        // replace the current camera with the player camera
        app().activateCamera(camera);

        // display information about the game pads
        log(to_string(Input::getConnectedJoypads()), "connected gamepad(s)");
        for (int i = 0; i < Input::getConnectedJoypads(); i++) {
            if (Input::isGamepad(i)) {
                gamepad = i;
                break;
            }
        }
        if (gamepad != -1) {
            log("Using gamepad", Input::getJoypadName(gamepad));
        }
        //printTree();
    }

    // release the mouse to access the menus
    void releaseMouse() {
        Input::setMouseMode(MOUSE_MODE_VISIBLE);
        mouseCaptured = false;
    }

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
    void captureMouse() {
        if (!mouseCaptured) {
            Input::setMouseMode(MOUSE_MODE_HIDDEN_CAPTURED);
            mouseCaptured = true;
        }
    }

    // signal handler called when the camera collide or stop collinding with something
    void onCameraCollision(const Collision *collision) {
        cameraCollisionTarget  = collision->object;
        cameraCollisionCounter = cameraCollisionCounterMax;
    }
};

const Signal::signal Player::on_push_pull = "on_push_pull";