module;
#include "libraries.h"

export module Example:Player;

import :Layers;

export class Player : public Character {
public:
    static const Signal::signal on_push_pull;

    struct PushOrPullAction : public Signal::Parameters {
        bool push;
        bool pull;
    };

    Player():
        Character{
            make_shared<BoxShape>(vec3{0.8f, 2.0f, 0.8f}),
            Layers::PLAYER,
            Layers::WORLD | Layers::BODIES
        },
        cameraCollisionCounterMax{100} {
    }

    bool onInput(InputEvent& event) override {
        if (mouseCaptured && (event.getType() == INPUT_EVENT_MOUSE_MOTION)) {
            auto& eventMouseMotion = dynamic_cast<InputEventMouseMotion&>(event);
            auto yRot = -eventMouseMotion.getRelativeX() * mouseSensitivity;
            rotateY(yRot);
            cameraPivot->rotateX(eventMouseMotion.getRelativeY() * mouseSensitivity * mouseInvertedAxisY);
            cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX(), maxCameraAngleDown, maxCameraAngleUp));
            return true;
        }
        if ((event.getType() == INPUT_EVENT_MOUSE_BUTTON) && (!mouseCaptured)) {
            auto& eventMouseButton = dynamic_cast<InputEventMouseButton&>(event);
            if (!eventMouseButton.isPressed()) {
                captureMouse();
                return true;
            }
        }
        if ((event.getType() == INPUT_EVENT_KEY) && mouseCaptured) {
            auto& eventKey = dynamic_cast<InputEventKey&>(event);
            if ((eventKey.getKey() == KEY_ESCAPE) && !eventKey.isPressed()) {
                releaseMouse();
                return true;
            }
            auto params = PushOrPullAction{
                .push = (eventKey.getKey() == KEY_P) && eventKey.isPressed(),
                .pull = (eventKey.getKey() == KEY_O) && eventKey.isPressed()
            };
            emit(on_push_pull, &params);
        }
        if ((event.getType() == INPUT_EVENT_GAMEPAD_BUTTON) && mouseCaptured) {
            auto& eventGamepadButton = dynamic_cast<InputEventGamepadButton&>(event);
            if ((eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_START) && !eventGamepadButton.isPressed()) {
                releaseMouse();
                return true;
            }
            auto params = PushOrPullAction{
                .push = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_RB) && eventGamepadButton.isPressed(),
                .pull = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_LB) && eventGamepadButton.isPressed()
            };
            emit(on_push_pull, &params);
        }
        return false;
    }

    void onPhysicsProcess(float delta) override {
        if ((cameraCollisionTarget != nullptr) && ((cameraInTween == nullptr) || (!cameraInTween->isRunning()))) {
            const auto pos = cameraAttachement->getPosition();
            const auto dest = vec3{0.0f, 0.0f, -pos.z};
            if (cameraPivot->getPosition() != dest) {
                cameraInTween = cameraPivot->createPropertyTween(
                    PropertyTween<vec3>::Setter(&Node::setPosition),
                    cameraPivot->getPosition(),
                    dest,
                    0.5f);
                cameraPivot->killTween(cameraOutTween);
            }
        }

        previousState = currentState;
        currentState = State{};
        vec2 input = VEC2ZERO;
        if (gamepad != -1) {
            input = Input::getGamepadVector(gamepad, GAMEPAD_AXIS_LEFT);
        }
        if (input == VEC2ZERO) {
            input = Input::getKeyboardVector(KEY_A, KEY_D, KEY_W, KEY_S);
        }

        // Determine news basic velocity
        auto onGround = isOnGround();
        auto currentVerticalVelocity = dot(getVelocity(), getUpVector()) * getUpVector();
        if (onGround) {
            currentState.velocity = getGroundVelocity();
            if (Input::isKeyPressed(KEY_SPACE) || Input::isGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_A)) {
                currentState.velocity += (jumpSpeed + currentMovementSpeed / 2.0f) * getUpVector();
            }
        }
        else {
            currentState.velocity = currentVerticalVelocity;
            // Apply gravity
            currentState.velocity += app().getGravity() * getUpVector() * delta;
        }

        if (input != VEC2ZERO) {
            if (!mouseCaptured) { captureMouse(); }
            if (onGround) {
                // Apply user input
                auto direction = TRANSFORM_BASIS * vec3{input.x, 0, input.y};
                if (currentMovementSpeed == 0) {
                    currentMovementSpeed = minMovementsSpeed;
                }
                else {
                    currentMovementSpeed += acceleration * delta;
                    currentMovementSpeed = std::min(currentMovementSpeed, maxMovementsSpeed);
                }
                currentState.velocity += direction * currentMovementSpeed;
            }
        }
        else {
            currentMovementSpeed = 0;
        }
        if (!onGround) {
            // Preserve horizontal velocity
            auto currentHorizontalVelocity = previousState.velocity - currentVerticalVelocity;
            currentState.velocity += currentHorizontalVelocity;
        }

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

    void onProcess(float alpha) override {
        if ((cameraCollisionTarget != nullptr) && (!cameraCollisionSensor->wereInContact(cameraCollisionTarget))) {
            cameraCollisionCounter -= 1;
            if (cameraCollisionCounter == 0) {
                cameraOutTween = cameraPivot->createPropertyTween(
                    PropertyTween<vec3>::Setter(&Node::setPosition),
                    cameraPivot->getPosition(),
                    VEC3ZERO,
                    0.5f);
                cameraPivot->killTween(cameraInTween);
                cameraCollisionTarget = nullptr;
            }
        }

        if (currentState.velocity != VEC3ZERO) {
            // set interpolated velocity
            setVelocity(previousState.velocity * (1.0f - alpha) + currentState.velocity * alpha);
        }
        else {
            setVelocity(VEC3ZERO);
        }
        if (currentState.lookDir != VEC2ZERO) {
            auto interpolatedLookDir = previousState.lookDir * (1.0f - alpha) + currentState.lookDir * alpha;
            auto yRot = -interpolatedLookDir.x * 2.0f;
            rotateY(yRot);
            cameraPivot->rotateX(interpolatedLookDir.y * keyboardInvertedAxisY);
            cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX(), maxCameraAngleDown, maxCameraAngleUp));
        }
    }

    void onReady() override {
        captureMouse();

        model = Loader::loadModelFromFile("res/models/capsule.glb", true);
        model->setPosition({0.0, -1.8 / 2.0, 0.0});
        addChild(model);

        cameraAttachement = make_shared<Node>("cameraAttachement");
        auto attachementZOffset = 2.0f;
        auto attachementYOffset = 1.6f;
        cameraAttachement->setPosition({0.0, attachementYOffset, attachementZOffset});
        addChild(cameraAttachement);

        cameraCollisionSensor = make_shared<CollisionArea>(
            make_shared<SphereShape>(attachementZOffset - 0.6f),
            Layers::WORLD | Layers::BODIES,
            "cameraCollisionNode"
        );
        cameraCollisionSensor->setPosition({0.0, attachementYOffset, attachementZOffset / 2.0f - 0.5f});
        cameraCollisionSensor->connect(CollisionObject::on_collision_starts, this,
                                       Signal::Handler(&Player::onCameraCollision));
        cameraCollisionSensor->connect(CollisionObject::on_collision_persists, this,
                                       Signal::Handler(&Player::onCameraCollision));
        addChild(cameraCollisionSensor);

        cameraPivot = make_shared<Node>("cameraPivot");
        cameraAttachement->addChild(cameraPivot);

        camera = make_shared<Camera>("Player camera");
        camera->setPerspectiveProjection(75.0, 0.1, 200.0);
        cameraPivot->addChild(camera);
        app().activateCamera(camera);

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

private:
    struct State {
        vec3 velocity = VEC3ZERO;
        vec2 lookDir = VEC2ZERO;
        State& operator=(const State& other) = default;
    };

    const float minMovementsSpeed = 1.5f;
    const float maxMovementsSpeed = 4.0f;
    const float acceleration = 2.0f;
    const float jumpSpeed = 5.5f;
    const float mouseSensitivity = 0.008f;
    const float viewSensitivity = 0.2f;
    const float maxCameraAngleUp = radians(60.0);
    const float maxCameraAngleDown = -radians(45.0);

    int gamepad{-1};
    bool mouseCaptured{false};
    float mouseInvertedAxisY{1.0};
    float keyboardInvertedAxisY{1.0};
    State previousState;
    State currentState;
    float currentMovementSpeed;
    shared_ptr<Node> model;

    shared_ptr<Camera> camera;
    shared_ptr<Node> cameraPivot;
    shared_ptr<Node> cameraAttachement;
    shared_ptr<CollisionArea> cameraCollisionSensor;
    CollisionObject* cameraCollisionTarget{nullptr};
    int cameraCollisionCounterMax;
    int cameraCollisionCounter{0};
    shared_ptr<Tween> cameraInTween;
    shared_ptr<Tween> cameraOutTween;

    void captureMouse() {
        if (!mouseCaptured) {
            Input::setMouseMode(MOUSE_MODE_HIDDEN_CAPTURED);
            mouseCaptured = true;
        }
    }

    void releaseMouse() {
        Input::setMouseMode(MOUSE_MODE_VISIBLE);
        mouseCaptured = false;
    }

    void onCameraCollision(CollisionObject::Collision* collision) {
        cameraCollisionTarget = collision->object;
        cameraCollisionCounter = cameraCollisionCounterMax;
    }
};

const Signal::signal Player::on_push_pull = "on_push_pull";
