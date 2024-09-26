module;
#include "libraries.h"
import Z0;
using namespace z0;

export module Example:AddRemoveChildMainScene;

export class AddRemoveChildMainScene : public Node {
public:
    AddRemoveChildMainScene(): Node{"Main Scene"} {
    };

    void onReady() override {
        camera1 = make_shared<Camera>("Camera 1");
        camera1->setPosition({0.0f, 0.0f, 1.0f});
        addChild(camera1);
        camera2 = make_shared<Camera>("Camera 2");
        camera2->rotateX(radians(-40.0));
        camera2->setPosition({0.0f, 10.0f, 1.0f});
        addChild(camera2);
        app().activateCamera(camera1);
        currentCamera = camera1;

        sphereModel = Loader::loadModelFromFile("res/models/sphere.glb");
        crateModel = Loader::loadModelFromFile("res/models/crate.glb");

        /*crateModel->setPosition({-5.0f, 0.0f, -10.0f});
        addChild(crateModel);
        rotatingNodes.push_back(crateModel);

        sphereModel->setPosition({5.0f, 0.0f, -10.0f});
        addChild(sphereModel);
        rotatingNodes.push_back(sphereModel);*/
    }

    bool onInput(InputEvent& event) override {
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
        const auto angle = delta * radians(90.0f) / 2;
        for (auto& crate : rotatingNodes) {
            crate->rotateY(angle);
            crate->rotateX(angle);
        }
    }

    void onEnterScene() override {
        menu = make_shared<GWindow>(Rect{0, 1000 - 550, 150, 500});
        app().addWindow(menu);
        menu->getWidget().setPadding(5);
        menu->getWidget().setDrawBackground(false);

        const auto menuAdd = make_shared<GButton>();
        menu->getWidget().add(menuAdd, GWidget::TOPCENTER, "200,40");
        menuAdd->add(make_shared<GText>("[ENTER] Add node"), GWidget::CENTER);
        menuAdd->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuAdd));
        const auto menuRemove = make_shared<GButton>();
        menu->getWidget().add(menuRemove, GWidget::TOPCENTER, "200,40");
        menuRemove->add(make_shared<GText>("[BACKSPACE] Remove node"), GWidget::CENTER);
        menuRemove->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuRemove));
        const auto menuCamera = make_shared<GButton>();
        menu->getWidget().add(menuCamera, GWidget::TOPCENTER, "200,40");
        menuCamera->add(make_shared<GText>("[SPACE] Toggle camera"), GWidget::CENTER);
        menuCamera->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuCamera));
    }

    void onExitScene() override {
        app().removeWindow(menu);
    }

private:
    shared_ptr<Node> crateModel;
    shared_ptr<Node> sphereModel;
    list<shared_ptr<Node>> rotatingNodes;
    shared_ptr<Camera> camera1;
    shared_ptr<Camera> camera2;
    shared_ptr<Camera> currentCamera;
    shared_ptr<GWindow> menu;

    void onMenuAdd(GEventClick* e = nullptr) {
        const auto newNode = (randomi(1) == 0) ? crateModel->duplicate() : sphereModel->duplicate();
        newNode->setPosition({randomf(10.0f) - 5, randomf(10.0f) - 5, -10.0f});
        if (addChild(newNode)) { rotatingNodes.push_back(newNode); }
    }

    void onMenuRemove(GEventClick* e = nullptr) {
        if (removeChild(rotatingNodes.back())) { rotatingNodes.pop_back(); }
    }

    void onMenuCamera(GEventClick* e = nullptr) {
        if (camera1->isActive()) {
            currentCamera = camera2;
            app().activateCamera(camera2);
        }
        else {
            currentCamera = camera1;
            app().activateCamera(camera1);
        }
    }
};
