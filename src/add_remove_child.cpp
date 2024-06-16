#include "includes.h"
#include "add_remove_child.h"

void AddRemoveChildMainScene::onEnterScene() {
    menu = make_shared<GWindow>(Rect{0, 1000-550, 200, 500});
    app().addWindow(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setFont(make_shared<Font>(menu->getWidget().getFont()->getFontName(),
                                                menu->getWidget().getFont()->getFontSize() / 1.5));
    menu->getWidget().setDrawBackground(false);
    
    auto menuAdd = make_shared<GButton>();
    menu->getWidget().add(menuAdd, GWidget::TOPCENTER, "200,40");
    menuAdd->add(make_shared<GText>("[ENTER] Add node"), GWidget::CENTER);
    menuAdd->connect(GEvent::OnClick, this, GEventFunction(&AddRemoveChildMainScene::onMenuAdd));
    auto menuRemove = make_shared<GButton>();
    menu->getWidget().add(menuRemove, GWidget::TOPCENTER, "200,40");
    menuRemove->add(make_shared<GText>("[BACKSPACE] Remove node"), GWidget::CENTER);
    menuRemove->connect(GEvent::OnClick, this, GEventFunction(&AddRemoveChildMainScene::onMenuRemove));
    auto menuCamera = make_shared<GButton>();
    menu->getWidget().add(menuCamera, GWidget::TOPCENTER, "200,40");
    menuCamera->add(make_shared<GText>("[SPACE] Toggle camera"), GWidget::CENTER);
    menuCamera->connect(GEvent::OnClick, this, GEventFunction(&AddRemoveChildMainScene::onMenuCamera));
}

void AddRemoveChildMainScene::onExitScene() {
    app().removeWindow(menu);
}

void AddRemoveChildMainScene::onMenuAdd(GWidget*, GEvent*) {
    auto newNode = (randomi(1) == 0) ? crateModel->duplicate() : sphereModel->duplicate();
    newNode->setPosition({randomf(10.0f) - 5, randomf(10.0f) - 5, -10.0f});
    if (addChild(newNode))  { rotatingNodes.push_back(newNode); }
}

void AddRemoveChildMainScene::onMenuRemove(GWidget*, GEvent*) {
    if (removeChild(rotatingNodes.back())) { rotatingNodes.pop_back(); }
}

void AddRemoveChildMainScene::onMenuCamera(GWidget*, GEvent*) {
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