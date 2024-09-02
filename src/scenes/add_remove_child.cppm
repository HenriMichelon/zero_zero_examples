module;
#include <z0/z0.h>

export module Example:AddRemoveChildMainScene;

export class AddRemoveChildMainScene: public Node {
public:
    AddRemoveChildMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    bool onInput(InputEvent& event) override;
    void onPhysicsProcess(float delta) override;
    void onEnterScene() override;
    void onExitScene() override;
private:
    shared_ptr<Node> crateModel;
    shared_ptr<Node> sphereModel;
    list<shared_ptr<Node>> rotatingNodes;
    shared_ptr<Camera> camera1;
    shared_ptr<Camera> camera2;
    shared_ptr<Camera> currentCamera;
    shared_ptr<GWindow> menu;

    void onMenuAdd(GEventClick*e=nullptr);
    void onMenuRemove(GEventClick*e=nullptr);
    void onMenuCamera(GEventClick*e=nullptr);
};

void AddRemoveChildMainScene::onEnterScene() {
    menu = make_shared<GWindow>(Rect{0, 1000-550, 150, 500});
    app().addWindow(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setDrawBackground(false);

    auto menuAdd = make_shared<GButton>();
    menu->getWidget().add(menuAdd, GWidget::TOPCENTER, "200,40");
    menuAdd->add(make_shared<GText>("[ENTER] Add node"), GWidget::CENTER);
    menuAdd->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuAdd));
    auto menuRemove = make_shared<GButton>();
    menu->getWidget().add(menuRemove, GWidget::TOPCENTER, "200,40");
    menuRemove->add(make_shared<GText>("[BACKSPACE] Remove node"), GWidget::CENTER);
    menuRemove->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuRemove));
    auto menuCamera = make_shared<GButton>();
    menu->getWidget().add(menuCamera, GWidget::TOPCENTER, "200,40");
    menuCamera->add(make_shared<GText>("[SPACE] Toggle camera"), GWidget::CENTER);
    menuCamera->connect(GEvent::OnClick, this, Signal::Handler(&AddRemoveChildMainScene::onMenuCamera));
}

void AddRemoveChildMainScene::onExitScene() {
    app().removeWindow(menu);
}

void AddRemoveChildMainScene::onMenuAdd(GEventClick*) {
    auto newNode = (randomi(1) == 0) ? crateModel->duplicate() : sphereModel->duplicate();
    newNode->setPosition({randomf(10.0f) - 5, randomf(10.0f) - 5, -10.0f});
    if (addChild(newNode))  { rotatingNodes.push_back(newNode); }
}

void AddRemoveChildMainScene::onMenuRemove(GEventClick*) {
    if (removeChild(rotatingNodes.back())) { rotatingNodes.pop_back(); }
}

void AddRemoveChildMainScene::onMenuCamera(GEventClick*) {
    if (camera1->isActive()) {
            currentCamera = camera2;
        app().activateCamera(camera2);
    } else {
        currentCamera = camera1;
        app().activateCamera(camera1);
    }
}

void AddRemoveChildMainScene::onReady() {
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

bool AddRemoveChildMainScene::onInput(InputEvent& event) {
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

void AddRemoveChildMainScene::onPhysicsProcess(float delta) {
    auto angle = delta * radians(90.0f) / 2;
    for (auto& crate: rotatingNodes) {
        crate->rotateY(angle);
        crate->rotateX(angle);
    }
}