module;
#include "libraries.h"

export module example.AddRemoveChildMainScene;

export class AddRemoveChildMainScene : public Node {
public:
    AddRemoveChildMainScene();

    void onReady() override;

    // handle all the player actions
    bool onInput(InputEvent &event) override ;

    void onPhysicsProcess(float delta) override;

    void onEnterScene() override;

    void onExitScene() override ;

private:
    // mesh for the first model
    shared_ptr<Node>       crateModel;
    // mesh for the second model
    shared_ptr<Node>       sphereModel;
    // all the nodes added by the player
    list<shared_ptr<Node>> rotatingNodes;
    // default scene camera
    shared_ptr<Camera>     camera1;
    // optional scene camera
    shared_ptr<Camera>     camera2;
    // used to toggle the cameras
    shared_ptr<Camera>     currentCamera;
    // main scene menu
    shared_ptr<ui::Window>    menu;

    // add a new node into the scene
    // randomly select a model
    void onMenuAdd(ui::EventClick *e = nullptr);

    // remove the last added node
    void onMenuRemove(ui::EventClick *e = nullptr);

    // toggle cameras
    void onMenuCamera(ui::EventClick *e = nullptr);
};
