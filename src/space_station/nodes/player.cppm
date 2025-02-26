module;
#include "libraries.h"

export module game.Player;

import layers;

export namespace space_station {

    class MyPlayer : public Character {
    public:
        MyPlayer();

        bool onInput(InputEvent &event) override;

        void onPhysicsProcess(float delta) override;

        void onProcess(float alpha) override;

        void onReady() override;

        void onExitScene() override;

        void releaseMouse();

    protected:
        bool cameraCollisions{false};

    private:
        // movement and view state
        struct PlayerState {
            vec3 velocity{VEC3ZERO};
            PlayerState &operator=(const PlayerState &other) = default;
        };
        struct ViewSate {
            vec2 lookDir{VEC2ZERO};
            ViewSate &operator=(const ViewSate &other) = default;
        };

        PlayerState previousState;
        PlayerState currentState;
        ViewSate previousViewState;
        ViewSate currentViewState;

        // movement starting speed
        const float minMovementsSpeed{1.5f};
        // maximum movement speed
        const float maxMovementsSpeed{4.0f};
        // acceleration from minMovementsSpeed to maxMovementsSpeed
        const float acceleration{2.0f};
        // jump height
        const float jumpSpeed{3.0f};
        // view rotation speed when the mouse is used
        const float mouseSensitivity{0.05f};
        // view rotation speed when the gamepad or keyboard is used
        const float viewSensitivity{0.15f};
        // max camera vertical angle
        const float maxCameraAngleUp{radians(60.0)};
        // min camera vertical angle
        const float maxCameraAngleDown{-radians(45.0)};

        // current game pad id
        int gamepad{-1};
        // did the mouse is captured ?
        bool mouseCaptured{false};
        // invert the vertical mouse axis
        float mouseInvertedAxisY{-1.0};
        // invert the vertical keyboard axis
        float keyboardInvertedAxisY{1.0};
        // current player speed
        float currentMovementSpeed{minMovementsSpeed};
        //
        vec2 previousMousePosition{VEC2ZERO};
        // player model
        shared_ptr<Node> model;
        // player camera
        shared_ptr<Camera> camera;
        // pivot to rotate the camera
        shared_ptr<Node> cameraPivot;
        // camera pivot attachment point
        shared_ptr<Node> cameraAttachment;
        //
        shared_ptr<ui::Window> hud;
        //
        shared_ptr<ui::Text> displayInfoText;

        // capture the mouse to allow the use of the keyboard & gamepad
        void captureMouse();

    };

}