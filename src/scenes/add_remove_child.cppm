module;
#include "libraries.h"

export module Example:AddRemoveChildMainScene;

export class AddRemoveChildMainScene : public Node {
public:
    AddRemoveChildMainScene():
        Node{"Main Scene"} {
    }

    void onReady() override {
        // create the default scene camera
        camera1 = make_shared<Camera>("Camera 1");
        camera1->setPosition({0.0f, 0.0f, 1.0f});
        addChild(camera1);
        // create the optional scene camera
        camera2 = make_shared<Camera>("Camera 2");
        camera2->rotateX(radians(-40.0));
        camera2->setPosition({0.0f, 10.0f, 1.0f});
        addChild(camera2);
        // replace the main menu camera by the default camera
        app().activateCamera(camera1);
        currentCamera = camera1;

        // load all the models from files
        sphereModel = Loader::loadModelFromFile("res/models/sphere.glb");
        crateModel  = Loader::loadModelFromFile("res/models/crate.glb");
    }

    // handle all the player actions
    bool onInput(InputEvent &event) override {
        bool consumed = false;
        if (Input::isKeyJustPressed(KEY_ENTER)) {
            onMenuAdd();
            consumed = true;
        }
        if (Input::isKeyJustPressed(KEY_BACKSPACE)) {
            onMenuRemove();
            consumed = true;
        }
        if (Input::isKeyJustPressed(KEY_SPACE)) {
            onMenuCamera();
            consumed = true;
        }
        return consumed;
    }

    void onPhysicsProcess(float delta) override {
        // rotates all the nodes
        const auto angle = delta * radians(90.0f) / 2;
        for (auto &crate : rotatingNodes) {
            crate->rotateY(angle);
            crate->rotateX(angle);
        }
    }

    void onEnterScene() override {
        // scene main menu
        menu = make_shared<GWindow>(Rect{0, 1000 - 550, 150, 500});
        app().add(menu);
        menu->getWidget().setPadding(5);
        menu->getWidget().setDrawBackground(false);
        // add a node button
        const auto menuAdd = make_shared<GButton>();
        menu->getWidget().add(menuAdd, GWidget::TOPCENTER, "200,40");
        menuAdd->add(make_shared<GText>("[ENTER] Add node"), GWidget::CENTER);
        menuAdd->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuAdd));
        // remove a node button
        const auto menuRemove = make_shared<GButton>();
        menu->getWidget().add(menuRemove, GWidget::TOPCENTER, "200,40");
        menuRemove->add(make_shared<GText>("[BACKSPACE] Remove node"), GWidget::CENTER);
        menuRemove->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuRemove));
        // toggle cameras button
        const auto menuCamera = make_shared<GButton>();
        menu->getWidget().add(menuCamera, GWidget::TOPCENTER, "200,40");
        menuCamera->add(make_shared<GText>("[SPACE] Toggle camera"), GWidget::CENTER);
        menuCamera->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuCamera));
    }

    void onExitScene() override {
        // remove the scene menu before returning to the main menu
        app().remove(menu);
    }

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
    shared_ptr<GWindow>    menu;

    // add a new node into the scene
    // randomly select a model
    void onMenuAdd(GEventClick *e = nullptr) {
        const auto newNode = (randomi(1) == 0) ? crateModel->duplicate() : sphereModel->duplicate();
        newNode->setPosition({randomf(10.0f) - 5, randomf(10.0f) - 5, -10.0f});
        if (addChild(newNode)) { rotatingNodes.push_back(newNode); }
    }

    // remove the last added node
    void onMenuRemove(GEventClick *e = nullptr) {
        if (removeChild(rotatingNodes.back())) { rotatingNodes.pop_back(); }
    }

    // toggle cameras
    void onMenuCamera(GEventClick *e = nullptr) {
        if (camera1->isActive()) {
            currentCamera = camera2;
            app().activateCamera(camera2);
        } else {
            currentCamera = camera1;
            app().activateCamera(camera1);
        }
    }
};
