module;
#include "libraries.h"

module Example;

import :TopBar;
import :TriangleMainScene;
import :AddRemoveChildMainScene;
import :PhysicsMainScene;
import :TerrainScene;
import :PlatformsScene;

GMenuEntry::GMenuEntry(const string &_label) :
    label{_label} {
    this->connect(GEvent::OnCreate, this, SignalHandler(&GMenuEntry::onCreate));
}

void GMenuEntry::onCreate() {
    const auto textLabel = make_shared<GText>(label);
    add(textLabel, CENTER);
    setSize(500, textLabel->getHeight() + 20);
}

void ExampleMainScene::onReady() {
    // create a camera to view the skybox
    addChild(make_shared<Camera>("Menu camera"));
    // create a beautiul skybox
    addChild(make_shared<Skybox>("res/textures/kloofendal_48d_partly_cloudy_puresky_2k.hdr"));
    // create a scene to start the other scenes
    scene = make_shared<Node>();
    addChild(scene);
}

// build the main menu and the top bar (displayed in the examples)
void ExampleMainScene::onEnterScene() {
    menu = make_shared<GWindow>(Rect{250, 0, 500, 1000});
    app().add(menu);
    menu->getWidget().setFont(make_shared<Font>(menu->getWidget().getFont()->getFontName(), 40));
    menu->getWidget().setDrawBackground(false);
    menu->getWidget().setPadding(10);
    float height = menu->getWidget().getPadding() * 8;

    const auto entryTriangle = make_shared<GMenuEntry>("Triangles & shaders");
    entryTriangle->connect(GEvent::OnClick, this, SignalHandler(&ExampleMainScene::onMenuTriangle));
    menu->getWidget().add(entryTriangle, GWidget::TOPCENTER);
    height += entryTriangle->getHeight();

    const auto entryAddRemoveChild = make_shared<GMenuEntry>("Add & remove child");
    entryAddRemoveChild->connect(GEvent::OnClick, this, SignalHandler(&ExampleMainScene::onMenuAddRemoveChild));
    menu->getWidget().add(entryAddRemoveChild, GWidget::TOPCENTER);
    height += entryAddRemoveChild->getHeight();

    const auto entryRaycast = make_shared<GMenuEntry>("Physics & RayCast");
    entryRaycast->connect(GEvent::OnClick, this, SignalHandler(&ExampleMainScene::onMenuRaycast));
    menu->getWidget().add(entryRaycast, GWidget::TOPCENTER);
    height += entryRaycast->getHeight();

    const auto entryTerrain = make_shared<GMenuEntry>("Scenes files");
    entryTerrain->connect(GEvent::OnClick, this, SignalHandler(&ExampleMainScene::onMenuTerrain));
    menu->getWidget().add(entryTerrain, GWidget::TOPCENTER);
    height += entryTerrain->getHeight();

    const auto entryQuit = make_shared<GMenuEntry>("Quit");
    entryQuit->connect(GEvent::OnClick, this, SignalHandler(&ExampleMainScene::onMenuQuit));
    menu->getWidget().add(entryQuit, GWidget::TOPCENTER);
    height += entryQuit->getHeight();

    menu->setHeight(height);
    menu->setY((VECTOR_SCALE.y - height) / 2);

    topbar = make_shared<TopBar>(this, SignalHandler(&ExampleMainScene::onMenu));
    app().add(topbar);

    menu->hide();
    topbar->show();
    // scene->addChild(make_shared<PlatformsScene>());
    onMenuTriangle(nullptr);
    // onMenuAddRemoveChild(nullptr);
    // onMenuRaycast(nullptr);
//    onMenuTerrain(nullptr);
}

void ExampleMainScene::onMenu(GEventClick *) {
    scene->removeAllChildren();
    topbar->hide();
    menu->show();
    app().setPaused(false);
}

void ExampleMainScene::onMenuQuit(GEventClick *) { app().quit(); }

void ExampleMainScene::onMenuTriangle(GEventClick *) const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<TriangleMainScene>());
}

void ExampleMainScene::onMenuAddRemoveChild(GEventClick *) const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<AddRemoveChildMainScene>());
}

void ExampleMainScene::onMenuRaycast(GEventClick *) const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<PhysicsMainScene>());
}

void ExampleMainScene::onMenuTerrain(GEventClick *) const {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<TerrainScene>());
}
