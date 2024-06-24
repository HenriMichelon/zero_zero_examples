#include "includes.h"
#include "layers.h"
#include "player.h"

const Signal::signal Player::on_push_pull = "on_push_pull";

Player::Player(int count): 
    Character{make_shared<BoxShape>(vec3{0.8f,2.0f, 0.8f}),
             Layers::PLAYER,
            Layers::WORLD | Layers::BODIES},
    cameraCollisionCounterMax{count} {
}

bool Player::onInput(InputEvent& event) {
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
            .pull = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_LB) && eventGamepadButton.isPressed() };
        emit(on_push_pull, &params);
    }
    return false;
}

void Player::onPhysicsProcess(float delta) {
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
            currentState.velocity += (jumpSpeed + currentMovementSpeed/2.0f) * getUpVector();
        }
    } else {
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
            } else {
                currentMovementSpeed += acceleration * delta;
                currentMovementSpeed = std::min(currentMovementSpeed, maxMovementsSpeed);
            }
            currentState.velocity += direction * currentMovementSpeed;
        }
    } else {
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

void Player::onProcess(float alpha) {
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
    } else {
        setVelocity(VEC3ZERO);
    }
    if (currentState.lookDir != VEC2ZERO) {
        auto interpolatedLookDir = previousState.lookDir * (1.0f-alpha) + currentState.lookDir * alpha;
         auto yRot = -interpolatedLookDir.x * 2.0f;
        rotateY(yRot);
        cameraPivot->rotateX(interpolatedLookDir.y * keyboardInvertedAxisY);
        cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX() , maxCameraAngleDown, maxCameraAngleUp));
    }
}

void Player::onReady() {
    captureMouse();

    model = Loader::loadModelFromFile("res/models/capsule.glb", true);
    model->setPosition({0.0, -1.8/2.0, 0.0});
    addChild(model);

    cameraAttachement = make_shared<Node>("cameraAttachement");
    auto attachementZOffset = 1.4f;
    auto attachementYOffset = 1.4f;
    cameraAttachement->setPosition({0.0, attachementYOffset, attachementZOffset});
    addChild(cameraAttachement);

    cameraCollisionSensor = make_shared<CollisionArea>(
        make_shared<SphereShape>(attachementZOffset),
        Layers::WORLD | Layers::BODIES,
        "cameraCollisionNode"
    );
    cameraCollisionSensor->setPosition({0.0, attachementYOffset, attachementZOffset / 2.0f});
    cameraCollisionSensor->connect(CollisionObject::on_collision_starts, this, Signal::Handler(&Player::onCameraCollision));
    cameraCollisionSensor->connect(CollisionObject::on_collision_persists, this, Signal::Handler(&Player::onCameraCollision));
    addChild(cameraCollisionSensor);

    cameraPivot = make_shared<Node>("cameraPivot");
    cameraAttachement->addChild(cameraPivot);

    camera = make_shared<Camera>("camera");
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

void Player::onCameraCollision(CollisionObject::Collision* collision) {
    cameraCollisionTarget = collision->object;
    cameraCollisionCounter = cameraCollisionCounterMax;
}

void Player::captureMouse() {
    if (!mouseCaptured) {
        Input::setMouseMode(MOUSE_MODE_HIDDEN_CAPTURED);
        mouseCaptured = true;
    }
}

void Player::releaseMouse() {
    Input::setMouseMode(MOUSE_MODE_VISIBLE);
    mouseCaptured = false;
}
