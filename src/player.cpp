#include "includes.h"
#include "player.h"
#include "layers.h"
#include "crate.h"

Player::Player(): Character{make_shared<BoxShape>(vec3{0.8f,2.0f, 0.8f}),
                            Layers::PLAYER,
                            Layers::WORLD | Layers::BODIES} {
}

bool Player::onInput(InputEvent& event) {
    if (mouseCaptured && (event.getType() == INPUT_EVENT_MOUSE_MOTION)) {
        auto& eventMouseMotion = dynamic_cast<InputEventMouseMotion&>(event);
        rotateY(-eventMouseMotion.getRelativeX() * mouseSensitivity);
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
        if ((eventKey.getKeyCode() == KEY_ESCAPE) && !eventKey.isPressed()) {
            releaseMouse();
            return true;
        }
        pushing = (eventKey.getKeyCode() == KEY_P) && eventKey.isPressed();
        pulling = (eventKey.getKeyCode() == KEY_O) && eventKey.isPressed();
    }
    if ((event.getType() == INPUT_EVENT_GAMEPAD_BUTTON) && mouseCaptured) {
        auto& eventGamepadButton = dynamic_cast<InputEventGamepadButton&>(event);
        if ((eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_START) && !eventGamepadButton.isPressed()) {
            releaseMouse();
            return true;
        }
        pushing = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_RB) && eventGamepadButton.isPressed();
        pulling = (eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_LB) && eventGamepadButton.isPressed();
    }
    return false;
}

void Player::onPhysicsProcess(float delta) {
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
    }
    // Apply gravity
    currentState.velocity += app().getGravity() * getUpVector() * delta;

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
    if (currentState.velocity != VEC3ZERO) {
        // set interpolated velocity
        setVelocity(previousState.velocity * (1.0f-alpha) + currentState.velocity * alpha);
    }
    if (currentState.lookDir != VEC2ZERO) {
        auto interpolatedLookDir = previousState.lookDir * (1.0f-alpha) + currentState.lookDir * alpha;
        rotateY(-interpolatedLookDir.x * 2.0f);
        cameraPivot->rotateX(interpolatedLookDir.y * keyboardInvertedAxisY);
        cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX() , maxCameraAngleDown, maxCameraAngleUp));
    }
    for (const auto& collision: previousCollisions) {
         collision.object->findFirstChild<MeshInstance>()->setOutlined(false);
    }
    previousCollisions.clear();
    for(const auto& collision : getCollisions()) {
        if (!isGround(collision.object)) {
            if (pushing || pulling) {
                collision.object->applyForce(
                    force * collision.normal * (pushing ? -1.0f : 1.0f),
                    collision.position);
            }
            auto* meshInstance = collision.object->findFirstChild<MeshInstance>();
            meshInstance->setOutlined(true);
            meshInstance->setOutlineMaterial(collisionOutlineMaterial);
            previousCollisions.push_back(collision);
        }
    }
}

void Player::onCollisionStarts(const Collision collision) {
    if (!isGround(collision.object)) {
        //log("Player start colliding with", collision.object->toString(), to_string(collision.object->getId()));
    }
}

void Player::onReady() {
    captureMouse();

    model = Loader::loadModelFromFile("res/models/capsule.glb", true);
    model->setPosition({0.0, -1.8/2.0, 0.0});
    addChild(model);

    cameraPivot = make_shared<Node>("CameraPivot");
    cameraPivot->setPosition({0.0, 1.8, 2.0});
    cameraPivot->rotateX(radians(-20.0));
    addChild(cameraPivot);

    camera = make_shared<Camera>();
    cameraPivot->addChild(camera);
    app().activateCamera(camera);

    log(to_string(Input::getConnectedJoypads()), "connected gamepad(s)");
    for (int i = 0; i < Input::getConnectedJoypads(); i++) {
        log(Input::getJoypadName(i));
    }
    for (int i = 0; i < Input::getConnectedJoypads(); i++) {
        if (Input::isGamepad(i)) {
            gamepad = i;
            break;
        }
    }
    if (gamepad != -1) {
        log("Using gamepad", Input::getJoypadName(gamepad));
    }

    collisionOutlineMaterial = make_shared<ShaderMaterial>(OutlineMaterials::get(0));
    collisionOutlineMaterial->setParameter(0, {0.0,1.0,0.0,1.0});
    collisionOutlineMaterial->setParameter(1, vec4{0.02});
    OutlineMaterials::add(collisionOutlineMaterial);

    //printTree();
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