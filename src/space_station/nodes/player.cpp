module;
#include "libraries.h"

module game.Player;

import game.Interactions;

namespace space_station {
    MyPlayer::MyPlayer():
        // the player have a physic body for the collisions
        Character{
            1.8f, 0.5f,
            PLAYER,
            "Player"
    } {
    }

    bool MyPlayer::onInput(InputEvent &event) {
        // if the mouse is used make it rotate horizontally 360°
        if (mouseCaptured && (event.getType() == InputEventType::MOUSE_MOTION)) {
            const auto &eventMouseMotion = dynamic_cast<InputEventMouseMotion &>(event);
            if (eventMouseMotion.getX() <= 0) {
                previousMousePosition = {app().getWindow().getWidth() -1, eventMouseMotion.getY()};
                Input::setMousePosition(previousMousePosition);
                return true;
            }
            if (eventMouseMotion.getX() >= (app().getWindow().getWidth()-1)) {
                previousMousePosition = {0, eventMouseMotion.getY()};
                Input::setMousePosition(previousMousePosition);
                return true;

            }
        }
        // capture the mouse on the first click if not already captured
        if ((event.getType() == InputEventType::MOUSE_BUTTON) && (!mouseCaptured)) {
            const auto &eventMouseButton = dynamic_cast<InputEventMouseButton &>(event);
            if (!eventMouseButton.isPressed()) {
                captureMouse();
                return true;
            }
        }
        // if the mouse is captured process the player keyboard events
        if ((event.getType() == InputEventType::KEY) && mouseCaptured) {
            const auto &eventKey = dynamic_cast<InputEventKey &>(event);
            // release the mouse with the ESC key
            if ((eventKey.getKey() == KEY_ESCAPE) && !eventKey.isPressed()) {
                releaseMouse();
                return true;
            }
        }
        // if the mouse is captured process the game pad events
        if ((event.getType() == InputEventType::GAMEPAD_BUTTON) && mouseCaptured) {
            const auto &eventGamepadButton = dynamic_cast<InputEventGamepadButton &>(event);
            // release the mouse with the START button
            if ((eventGamepadButton.getGamepadButton() == GamepadButton::START) && !eventGamepadButton.isPressed()) {
                releaseMouse();
                return true;
            }
        }
        // continue to propagate the event
        return false;
    }

    void MyPlayer::onPhysicsProcess(const float delta) {
        // reset the movements states
        previousState = currentState;
        currentState  = PlayerState{};

        // get the input vector, first from the game pad, second for the keyboard
        vec2 input = VEC2ZERO;
        if (gamepad != -1) {
            input = Input::getGamepadVector(gamepad, GamepadAxisJoystick::LEFT);
        }
        if (input == VEC2ZERO) {
            input = Input::getKeyboardVector(KEY_A, KEY_D, KEY_W, KEY_S);
        }

        // Determine news basic velocity
        const auto onGround = isOnGround();
        const auto currentVerticalVelocity = dot(getVelocity(), getUpVector()) * getUpVector();
        if (onGround) {
            // we move if the player is on the ground or on an object
            currentState.velocity = getGroundVelocity();
            // jump !
            if (Input::isKeyPressed(KEY_SPACE) || Input::isGamepadButtonPressed(gamepad, GamepadButton::A)) {
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
                // check for stairs to slow down
                bool slowdown = false;
                for (const auto & collision : getCollisions()) {
                    if (collision.object->isInGroup("stairs")) {
                        slowdown = collision.normal.y > 0.9;
                        if (slowdown) { break; }
                    }
                }
                // move
                currentState.velocity += direction * currentMovementSpeed * (slowdown ? 0.7f : 1.0f);
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

        // Apply movements
        setVelocity(previousState.velocity * (1.0f - delta) + currentState.velocity * delta);

    }

    void MyPlayer::onProcess(const float alpha) {
        // If the mouse is captured we can use the game pad, the keyboard or the mouse
        // to rotate the view
        if (mouseCaptured) {
            previousViewState = currentViewState;
            currentViewState  = ViewSate{};
            float axisInvertion;
            float sensitivity;

            vec2 inputDir = VEC2ZERO;
            if (gamepad != -1) {
                inputDir = Input::getGamepadVector(gamepad, GamepadAxisJoystick::RIGHT);
            }
            if (inputDir == VEC2ZERO) {
                inputDir = Input::getKeyboardVector(KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN);
            }
            if (inputDir != VEC2ZERO) {
                currentViewState.lookDir = inputDir * viewSensitivity;
                axisInvertion = keyboardInvertedAxisY;
                sensitivity = viewSensitivity;
            }
            const auto mousePos = Input::getMousePosition();
            if ((currentViewState.lookDir == VEC2ZERO) && (mousePos != previousMousePosition)) {
                const auto inputDir = mousePos - previousMousePosition;
                previousMousePosition = mousePos;
                currentViewState.lookDir =  inputDir * mouseSensitivity;
                axisInvertion = mouseInvertedAxisY;
                sensitivity = mouseSensitivity;
            }

            // rotate the view
            if (currentViewState.lookDir != VEC2ZERO) {
                const auto interpolatedLookDir = previousViewState.lookDir * (1.0f - alpha) + currentViewState.lookDir * alpha;
                const auto yRot = -interpolatedLookDir.x * sensitivity;
                rotateY(yRot);
                cameraPivot->rotateX(interpolatedLookDir.y * axisInvertion * sensitivity);
                cameraPivot->setRotationX(std::clamp(cameraPivot->getRotationX(), maxCameraAngleDown, maxCameraAngleUp));
            }
        }
    }

    void MyPlayer::onReady() {
        previousMousePosition = Input::getMousePosition();
        // We can climb the stairs
        setMaxSlopeAngle(55.0f);

        // we capture the mouse at startup, ready to play !
        captureMouse();

        // create the player node
        model = Loader::load("app://res/models/capsule.glb");
        addChild(model);

        // create the attachment point for the camera pivot
        cameraAttachment       = make_shared<Node>("cameraAttachment");
        auto attachmentZOffset = -0.5f;
        auto attachmentYOffset = 1.7f;
        auto attachmentXOffset = 0.0f;
        // auto attachmentZOffset = 1.5f;
        // auto attachmentYOffset = 1.5f;
        // auto attachmentXOffset = 0.5f;
        cameraAttachment->setPosition({attachmentXOffset, attachmentYOffset, attachmentZOffset});
        addChild(cameraAttachment);

        // create the pivot used to rotate the camera
        cameraPivot = make_shared<Node>("cameraPivot");
        cameraAttachment->addChild(cameraPivot);

        // create the player camera and attach it to the pivot
        camera = make_shared<Camera>("Player camera");
        camera->setPerspectiveProjection(65.0, 0.01, 50.0);
        cameraPivot->addChild(camera);
        // replace the current camera with the player camera
        app().activateCamera(camera);

        // create the raycast used to detect interactions
        const auto interactions = make_shared<Interactions>(camera);
        interactions->setPosition({0.0f, 1.5f, -0.5});
        addChild(interactions);
        interactions->connect(Interactions::on_display_info, [this](void*param) {
            const auto* node = static_cast<Node*>(param);
            displayInfoText->setText("[E] " + node->getName());
            hud->add(displayInfoText, ui::Widget::CENTER);
        });
        interactions->connect(Interactions::on_hide_info, [this]() {
            hud->remove(displayInfoText);
        });

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

        hud = make_shared<ui::Window>(ui::FULLSCREEN);
        app().add(hud);
        hud->getWidget().setPadding(10.0f);

        displayInfoText =  make_shared<ui::Text>("XXXXXXXXXXXXXXXXX");
        displayInfoText->setFont(make_shared<Font>(displayInfoText->getFont(), 40));
        displayInfoText->setTextColor(vec4{0.3, 0.3, 1.0, 1.0f});
    }

    void MyPlayer::releaseMouse() {
        Input::setMouseMode(MouseMode::VISIBLE);
        mouseCaptured = false;
    }

    void MyPlayer::captureMouse() {
        if (!mouseCaptured) {
            Input::setMouseMode(MouseMode::HIDDEN_CAPTURED);
            mouseCaptured = true;
        }
    }
}