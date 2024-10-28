module;
#include "libraries.h"

module Example;

import :AddRemoveChildMainScene;

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
    app().activateCamera(camera1);
    currentCamera = camera1;

    // load all the models from files
    sphereModel = Loader::loadModelFromFile("app://res/models/sphere.glb");
    crateModel  = Loader::loadModelFromFile("app://res/models/crate.glb");

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
    menu = make_shared<GWindow>(Rect{0, 1000 - 550, 150, 500});
    app().add(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setDrawBackground(false);
    // add a node button
    const auto menuAdd = make_shared<GButton>();
    menu->getWidget().add(menuAdd, GWidget::TOPCENTER, "200,40");
    menuAdd->add(make_shared<GText>("[ENTER] Add node"), GWidget::CENTER);
    menuAdd->connect(GEvent::OnClick, this, SignalHandler(&AddRemoveChildMainScene::onMenuAdd));
    // remove a node button
    const auto menuRemove = make_shared<GButton>();
    menu->getWidget().add(menuRemove, GWidget::TOPCENTER, "200,40");
    menuRemove->add(make_shared<GText>("[BACKSPACE] Remove node"), GWidget::CENTER);
    menuRemove->connect(GEvent::OnClick, this, SignalHandler(&AddRemoveChildMainScene::onMenuRemove));
    // toggle cameras button
    const auto menuCamera = make_shared<GButton>();
    menu->getWidget().add(menuCamera, GWidget::TOPCENTER, "200,40");
    menuCamera->add(make_shared<GText>("[SPACE] Toggle camera"), GWidget::CENTER);
    menuCamera->connect(GEvent::OnClick, this, SignalHandler(&AddRemoveChildMainScene::onMenuCamera));
}

void AddRemoveChildMainScene::onExitScene() {
    // remove the scene menu before returning to the main menu
    app().remove(menu);
}

void AddRemoveChildMainScene::onMenuAdd(GEventClick *e) {
    const auto newNode = (randomi(1) == 0) ? crateModel->duplicate() : sphereModel->duplicate();
    newNode->setPosition({randomf(10.0f) - 5, randomf(10.0f) - 5, -10.0f});
    if (addChild(newNode)) { rotatingNodes.push_back(newNode); }
}

void AddRemoveChildMainScene::onMenuRemove(GEventClick *e) {
    if (removeChild(rotatingNodes.back())) { rotatingNodes.pop_back(); }
}

void AddRemoveChildMainScene::onMenuCamera(GEventClick *e) {
    if (camera1->isActive()) {
        currentCamera = camera2;
        app().activateCamera(camera2);
    } else {
        currentCamera = camera1;
        app().activateCamera(camera1);
    }
}
