#include "includes.h"
#include "topbar.h"
#include "nodes/player.h"
#include "example.h"
#include "scenes/triangle.h"
#include "scenes/add_remove_child.h"
#include "scenes/physics.h"
#include "scenes/terrain.h"

class GMenuEntry: public GButton {
public:
    explicit GMenuEntry(const string& _label): GButton{}, label{_label} {
        this->connect(GEvent::OnCreate, this, Signal::Handler(&GMenuEntry::onCreate));
    }

private:
    const string label;
    void onCreate() {
        auto textLabel = make_shared<GText>(label);
        add(textLabel, CENTER);
        setSize(500, textLabel->getHeight() + 20);
    }
};

void ExampleMainScene::onEnterScene() {
    menu = make_shared<GWindow>(Rect{250, 0, 500, 1000});
    app().addWindow(menu);
    menu->getWidget().setFont(make_shared<Font>(menu->getWidget().getFont()->getFontName(), 40));
    menu->getWidget().setDrawBackground(false);
    menu->getWidget().setPadding(10);
    float height = menu->getWidget().getPadding() * 8;

    auto entryTriangle = make_shared<GMenuEntry>("Triangles & shaders");
    entryTriangle->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuTriangle));
    menu->getWidget().add(entryTriangle, GWidget::TOPCENTER);
    height += entryTriangle->getHeight();

    auto entryAddRemoveChild = make_shared<GMenuEntry>("Add & remove child");
    entryAddRemoveChild->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuAddRemoveChild));
    menu->getWidget().add(entryAddRemoveChild, GWidget::TOPCENTER);
    height += entryAddRemoveChild->getHeight();

    auto entryRaycast = make_shared<GMenuEntry>("Physics & RayCast");
    entryRaycast->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuRaycast));
    menu->getWidget().add(entryRaycast, GWidget::TOPCENTER);
    height += entryRaycast->getHeight();

    auto entryTerrain = make_shared<GMenuEntry>("Scenes files");
    entryTerrain->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuTerrain));
    menu->getWidget().add(entryTerrain, GWidget::TOPCENTER);
    height += entryTerrain->getHeight();

    auto entryQuit = make_shared<GMenuEntry>("Quit");
    entryQuit->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuQuit));
    menu->getWidget().add(entryQuit, GWidget::TOPCENTER);
    height += entryQuit->getHeight();

    menu->setHeight(height);
    menu->setY((VECTOR_SCALE.y - height)/2);

    topbar = make_shared<TopBar>(this, Signal::Handler(&ExampleMainScene::onMenu));
    app().addWindow(topbar);
}

void ExampleMainScene::onReady() {
    addChild(make_shared<Camera>("Menu camera"));
    addChild(make_shared<Skybox>("res/textures/cubemap.png"));
    scene = make_shared<Node>();
    addChild(scene);
    auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
    log("VRAM usage after onReady():", to_string(usage)+"%");
}

void ExampleMainScene::onProcess(float alpha) {
    topbar->updateFPS();
}

void ExampleMainScene::onMenu(GEventClick*) {
    scene->removeAllChildren();
    topbar->hide();
    menu->show();
    app().setPaused(false);
}

void ExampleMainScene::onMenuQuit(GEventClick*) {
    app().quit();
}

void ExampleMainScene::onMenuTriangle(GEventClick*) {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<TriangleMainScene>());
}

void ExampleMainScene::onMenuAddRemoveChild(GEventClick*) {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<AddRemoveChildMainScene>());
}

void ExampleMainScene::onMenuRaycast(GEventClick*) {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<PhysicsMainScene>());
}

void ExampleMainScene::onMenuTerrain(GEventClick*) {
    menu->hide();
    topbar->show();
    scene->addChild(make_shared<TerrainScene>());
}

const ApplicationConfig applicationConfig {
    .appName = "Example App",
    .appDir = "..",
    .windowMode = WINDOW_MODE_WINDOWED_FULLSCREEN,
    .windowWidth = 1980,
    .windowHeight = 1020,
    .defaultFontName = "res/Signwood.ttf",
    .defaultFontSize = 25,
    .loggingMode = LOGGING_FILE | LOGGING_WINDOW
};

Z0_APP(applicationConfig, make_shared<ExampleMainScene>())