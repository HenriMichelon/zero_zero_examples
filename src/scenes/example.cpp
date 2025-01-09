module;
#include "libraries.h"

module example.MainScene;

import example.TopBar;
import example.TriangleMainScene;
import example.AddRemoveChildMainScene;
import example.PhysicsMainScene;

import game;

GMenuEntry::GMenuEntry(const string &_label) :
    label{_label} {
    this->connect(ui::Event::OnCreate, [this]{this->onCreate();});
}

void GMenuEntry::onCreate() {
    const auto textLabel = make_shared<ui::Text>(label);
    add(textLabel, CENTER);
    setSize(500, textLabel->getHeight() + 20);
}

void ExampleMainScene::onReady() {
    // create a camera to view the skybox
    addChild(make_shared<Camera>("Menu camera"));
    // create a beautiful skybox
    addChild(make_shared<Skybox>("app://res/textures/StandardCubeMap.jpg"));
    // create a scene to start the other scenes
    scene = make_shared<Node>();
    addChild(scene);
}

// build the main menu and the top bar (displayed in the examples)
void ExampleMainScene::onEnterScene() {
    menu = make_shared<ui::Window>(ui::Rect{250, 500, 500, 1000});
    Application::get().add(menu);
    menu->getWidget().setFont(make_shared<Font>(menu->getWidget().getFont().getFontName(), 40));
    menu->getWidget().setDrawBackground(false);
    menu->getWidget().setPadding(10);
    float height = menu->getWidget().getPadding() * 8;

    const auto entryTriangle = make_shared<GMenuEntry>("Triangles & shaders");
    entryTriangle->connect(ui::Event::OnClick, [this]{this->onMenuTriangle();});
    menu->getWidget().add(entryTriangle, ui::Widget::TOPCENTER);
    height += entryTriangle->getHeight();

    const auto entryAddRemoveChild = make_shared<GMenuEntry>("Add & remove child");
    entryAddRemoveChild->connect(ui::Event::OnClick, [this]{this->onMenuAddRemoveChild();});
    menu->getWidget().add(entryAddRemoveChild, ui::Widget::TOPCENTER);
    height += entryAddRemoveChild->getHeight();

    const auto entryRaycast = make_shared<GMenuEntry>("Physics & RayCast");
    entryRaycast->connect(ui::Event::OnClick, [this]{this->onMenuRaycast(); });
    menu->getWidget().add(entryRaycast, ui::Widget::TOPCENTER);
    height += entryRaycast->getHeight();

    const auto entryTerrain = make_shared<GMenuEntry>("Scenes files");
    entryTerrain->connect(ui::Event::OnClick, [this]{this->onMenuTerrain(); });
    menu->getWidget().add(entryTerrain, ui::Widget::TOPCENTER);
    height += entryTerrain->getHeight();

    const auto entryQuit = make_shared<GMenuEntry>("Quit");
    entryQuit->connect(ui::Event::OnClick, [this]{this->onMenuQuit(); });
    menu->getWidget().add(entryQuit, ui::Widget::TOPCENTER);
    height += entryQuit->getHeight();

    menu->setHeight(height);
    menu->setY((VECTOR_SCALE.y - height) / 2);

    topbar = make_shared<TopBar>(this, [this]{this->onMenu(); });
    Application::get().add(topbar);

    //menu->hide();
    //topbar->show();
    // onMenuTriangle();
    // onMenuAddRemoveChild();
    //onMenuRaycast();
    // onMenuTerrain();
}

void ExampleMainScene::onMenu() {
    scene->removeAllChildren();
    topbar->hide();
    menu->show();
    Application::get().setPaused(false);
}

void ExampleMainScene::onMenuQuit() { Application::get().quit(); }

void ExampleMainScene::onMenuTriangle() const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<TriangleMainScene>());
}

void ExampleMainScene::onMenuAddRemoveChild() const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<AddRemoveChildMainScene>());
}

void ExampleMainScene::onMenuRaycast() const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<PhysicsMainScene>());
}

void ExampleMainScene::onMenuTerrain() const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<space_station::MainScene>());
}
