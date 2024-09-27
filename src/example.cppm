module;
#include "libraries.h"
import Z0;
using namespace z0;

export module Example;

import :TopBar;
import :TriangleMainScene;
import :AddRemoveChildMainScene;
import :PhysicsMainScene;
import :TerrainScene;
export import :Crate;
export import :Player;
export import :Terrain;
export import :Platforms;

class GMenuEntry : public GButton {
public:
    explicit GMenuEntry(const string &_label) :
        label{_label} {
        this->connect(GEvent::OnCreate, this, Signal::Handler(&GMenuEntry::onCreate));
    }

private:
    const string label;

    void onCreate() {
        const auto textLabel = make_shared<GText>(label);
        add(textLabel, CENTER);
        setSize(500, textLabel->getHeight() + 20);
    }
};

export class ExampleMainScene : public Node {
public:
    ~ExampleMainScene() override = default;

    void onReady() override {
        addChild(make_shared<Camera>("Menu camera"));
        addChild(make_shared<Skybox>("res/textures/cubemap.png"));
        scene = make_shared<Node>();
        addChild(scene);
        const auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
        log("VRAM usage after onReady():", to_string(usage) + "%");
    }

    void onProcess(float alpha) override { topbar->updateFPS(); }

    void onEnterScene() override {
        menu = make_shared<GWindow>(Rect{250, 0, 500, 1000});
        app().addWindow(menu);
        menu->getWidget().setFont(make_shared<Font>(menu->getWidget().getFont()->getFontName(), 40));
        menu->getWidget().setDrawBackground(false);
        menu->getWidget().setPadding(10);
        float height = menu->getWidget().getPadding() * 8;

        const auto entryTriangle = make_shared<GMenuEntry>("Triangles & shaders");
        entryTriangle->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuTriangle));
        menu->getWidget().add(entryTriangle, GWidget::TOPCENTER);
        height += entryTriangle->getHeight();

        const auto entryAddRemoveChild = make_shared<GMenuEntry>("Add & remove child");
        entryAddRemoveChild->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuAddRemoveChild));
        menu->getWidget().add(entryAddRemoveChild, GWidget::TOPCENTER);
        height += entryAddRemoveChild->getHeight();

        const auto entryRaycast = make_shared<GMenuEntry>("Physics & RayCast");
        entryRaycast->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuRaycast));
        menu->getWidget().add(entryRaycast, GWidget::TOPCENTER);
        height += entryRaycast->getHeight();

        const auto entryTerrain = make_shared<GMenuEntry>("Scenes files");
        entryTerrain->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuTerrain));
        menu->getWidget().add(entryTerrain, GWidget::TOPCENTER);
        height += entryTerrain->getHeight();

        const auto entryQuit = make_shared<GMenuEntry>("Quit");
        entryQuit->connect(GEvent::OnClick, this, Signal::Handler(&ExampleMainScene::onMenuQuit));
        menu->getWidget().add(entryQuit, GWidget::TOPCENTER);
        height += entryQuit->getHeight();

        menu->setHeight(height);
        menu->setY((VECTOR_SCALE.y - height) / 2);

        topbar = make_shared<TopBar>(this, Signal::Handler(&ExampleMainScene::onMenu));
        app().addWindow(topbar);

        // menu->hide();
        // topbar->show();
        // scene->addChild(make_shared<TriangleMainScene>());
        onMenuTriangle(nullptr);
    }

private:
    shared_ptr<GWindow> menu;
    shared_ptr<TopBar>  topbar;
    shared_ptr<Node>    scene;

    void onMenu(GEventClick *) {
        scene->removeAllChildren();
        topbar->hide();
        menu->show();
        app().setPaused(false);
    }

    void onMenuQuit(GEventClick *) { app().quit(); }

    void onMenuTriangle(GEventClick *) const {
        menu->hide();
        topbar->show();
        scene->addChild(make_shared<TriangleMainScene>());
    }

    void onMenuAddRemoveChild(GEventClick *) const {
        menu->hide();
        topbar->show();
        scene->addChild(make_shared<AddRemoveChildMainScene>());
    }

    void onMenuRaycast(GEventClick *) const {
        menu->hide();
        topbar->show();
        scene->addChild(make_shared<PhysicsMainScene>());
    }

    void onMenuTerrain(GEventClick *) const {
        menu->hide();
        topbar->show();
        scene->addChild(make_shared<TerrainScene>());
    }
};
