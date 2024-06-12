#include "includes.h"
#include "player.h"
#include "layers.h"
#include "crate.h"

Player::Player(): Character{make_shared<BoxShape>(vec3{1.0f,2.0f, 1.0f}),
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
    }
    if ((event.getType() == INPUT_EVENT_GAMEPAD_BUTTON) && mouseCaptured) {
        auto& eventGamepadButton = dynamic_cast<InputEventGamepadButton&>(event);
        if ((eventGamepadButton.getGamepadButton() == GAMEPAD_BUTTON_START) && !eventGamepadButton.isPressed()) {
            releaseMouse();
            return true;
        }
    }
    return false;
}

void Player::onPhysicsProcess(float delta) {
    previousState = currentState;
    vec2 input = VEC2ZERO;
    if (gamepad != -1) {
        input = Input::getGamepadVector(gamepad, GAMEPAD_AXIS_LEFT);
    }
    if (input == VEC2ZERO) {
        input = Input::getKeyboardVector(KEY_A, KEY_D, KEY_W, KEY_S);
    }

    currentState = State{
        .velocity = getVelocity()
    };
    if (input != VEC2ZERO) {
        captureMouse();
        auto direction = TRANSFORM_BASIS * vec3{input.x, 0, input.y};
        currentState.velocity.x = direction.x * movementsSpeed;
        currentState.velocity.z = direction.z * movementsSpeed;
    }
    if ((Input::isKeyPressed(KEY_SPACE) || (Input::isGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_A)))
        && isOnGround()) {
        currentState.velocity.y = jumpHeight;
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
     gradient += gradientSpeed * delta;
    if (gradient > 1.0f) {
        gradient = 1.0f;
        gradientSpeed = -gradientSpeed;
    }
    if (gradient < 0.0f) {
        gradient = 0.0f;
        gradientSpeed = -gradientSpeed;
    }
    material->setParameter(0, vec4{gradient});
}

void Player::onProcess(float alpha) {
    if ((currentState.velocity != VEC3ZERO) && isOnGround()) {
        auto interpolatedVelocity = previousState.velocity * (1.0f-alpha) + currentState.velocity * alpha;
        setVelocity({interpolatedVelocity.x, interpolatedVelocity.y, interpolatedVelocity.z});
    }
    if (currentState.lookDir != VEC2ZERO) {
        auto interpolatedLookDir = previousState.lookDir * (1.0f-alpha) + currentState.lookDir * alpha;
        rotateY(-interpolatedLookDir.x * 2.0f);
        cameraPivot->rotateX(interpolatedLookDir.y * keyboardInvertedAxisY);
        cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX() , maxCameraAngleDown, maxCameraAngleUp));
    }
}

void Player::onCollisionStarts(PhysicsNode *node) {
    if (!isGround(node)) {
         if (previousCollision != nullptr) {
            previousCollision->setOutlined(false);
        }
        auto* meshInstance = node->findFirstChild<MeshInstance>();
        meshInstance->setOutlined(true);
        meshInstance->setOutlineMaterial(collisionOutlineMaterial);
        previousCollision = meshInstance;
    }
}

void Player::onReady() {
    captureMouse();

    model = Loader::loadModelFromFile("res/models/capsule.glb", true);
    model->setPosition({0.0, -1.8/2.0, 0.0});
    addChild(model);
    material = make_shared<ShaderMaterial>("examples/uv_gradient.frag");
    material->setParameter(0, vec4{0.0});
    auto mesh = dynamic_cast<MeshInstance*>(
        model->getNode("Sketchfab_model/root/GLTF_SceneRootNode/Sphere_0/Object_4").get())
        ->getMesh();
    mesh->setSurfaceMaterial(0, material);

    cameraPivot = make_shared<Node>("CameraPivot");
    cameraPivot->setPosition({0.0, 1.8, 2.0});
    cameraPivot->rotateX(radians(-20.0));
    addChild(cameraPivot);

    camera = make_shared<Camera>();
    cameraPivot->addChild(camera);
    app().activateCamera(camera);

    log(to_string(Input::getConnectedJoypads()), "connected gamepad(s)");
    for (int i = 0; i < Input::getConnectedJoypads(); i++) {
        log(Input::getGamepadName(i));
    }
    for (int i = 0; i < Input::getConnectedJoypads(); i++) {
        if (Input::isGamepad(i)) {
            gamepad = i;
            break;
        }
    }
    if (gamepad != -1) {
        log("Using gamepad", Input::getGamepadName(gamepad));
    }

    collisionOutlineMaterial = make_shared<ShaderMaterial>(OutlineMaterials::get().get(0));
    collisionOutlineMaterial->setParameter(0, {0.0,1.0,0.0,1.0});
    collisionOutlineMaterial->setParameter(1, vec4{0.05});
    OutlineMaterials::get().add(collisionOutlineMaterial);

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