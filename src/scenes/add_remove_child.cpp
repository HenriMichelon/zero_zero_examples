module;
#include "libraries.h"

module example.AddRemoveChildMainScene;

AddRemoveChildMainScene::AddRemoveChildMainScene():
    Node{"Main Scene"} {
}

void AddRemoveChildMainScene::onReady() {
    // add the global environement
    addChild(make_shared<Environment>(vec4{1.0, 1.0, 1.0, 0.2f}));
    // add the Sun
    const auto directionalLight1 = make_shared<DirectionalLight>(vec4{1.0f, 1.0f, 1.0f, 1.0f});
    directionalLight1->rotateY(radians(138.0f));
    directionalLight1->rotateX(radians(-45.0f));
    addChild(directionalLight1);
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
    Application::get().activateCamera(camera1);
    currentCamera = camera1;

    // load all the models from files
    sphereModel = Loader::load("app://res/models/sphere.zres");
    crateModel  = Loader::load("app://res/models/crate.zres");

    onMenuAdd(nullptr);
}

bool AddRemoveChildMainScene::onInput(InputEvent &event) {
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

void AddRemoveChildMainScene::onPhysicsProcess(const float delta) {
    // rotates all the nodes
    const auto angle = delta * radians(90.0f) / 2;
    for (const auto &crate : rotatingNodes) {
        crate->rotateY(angle);
        crate->rotateX(angle);
    }
}

void AddRemoveChildMainScene::onEnterScene() {
    // scene main menu
    menu = make_shared<ui::Window>(ui::Rect{0, 1000 - 550, 150, 500});
    Application::get().add(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setDrawBackground(false);
    // add a node button
    const auto menuAdd = make_shared<ui::Button>();
    menu->getWidget().add(menuAdd, ui::Widget::TOPCENTER, "200,40");
    menuAdd->add(make_shared<ui::Text>("[ENTER] Add node"), ui::Widget::CENTER);
    menuAdd->connect(ui::Event::OnClick, [this]{this->onMenuAdd();});
    // remove a node button
    const auto menuRemove = make_shared<ui::Button>();
    menu->getWidget().add(menuRemove, ui::Widget::TOPCENTER, "200,40");
    menuRemove->add(make_shared<ui::Text>("[BACKSPACE] Remove node"), ui::Widget::CENTER);
    menuRemove->connect(ui::Event::OnClick, [this]{this->onMenuRemove();});
    // toggle cameras button
    const auto menuCamera = make_shared<ui::Button>();
    menu->getWidget().add(menuCamera, ui::Widget::TOPCENTER, "200,40");
    menuCamera->add(make_shared<ui::Text>("[SPACE] Toggle camera"), ui::Widget::CENTER);
    menuCamera->connect(ui::Event::OnClick, [this]{this->onMenuCamera();});
}

void AddRemoveChildMainScene::onExitScene() {
    // remove the scene menu before returning to the main menu
    Application::get().remove(menu);
}

void AddRemoveChildMainScene::onMenuAdd(ui::EventClick *e) {
    const auto newNode = (randomi(1) == 0) ? crateModel->duplicate() : sphereModel->duplicate();
    newNode->setPosition({randomf(10.0f) - 5, randomf(10.0f) - 5, -10.0f});
    if (addChild(newNode)) { rotatingNodes.push_back(newNode); }
}

void AddRemoveChildMainScene::onMenuRemove(ui::EventClick *e) {
    if (removeChild(rotatingNodes.back())) { rotatingNodes.pop_back(); }
}

void AddRemoveChildMainScene::onMenuCamera(ui::EventClick *e) {
    if (camera1->isActive()) {
        currentCamera = camera2;
        Application::get().activateCamera(camera2);
    } else {
        currentCamera = camera1;
        Application::get().activateCamera(camera1);
    }
}
