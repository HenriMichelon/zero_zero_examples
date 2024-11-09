module;
#include "../libraries.h"

export module Example;

import :TopBar;
export import :Crate;
export import :Player;
export import :PlatformPlayer;
export import :Terrain;

// class for all the main menu entries
class GMenuEntry : public GButton {
public:
    explicit GMenuEntry(const string &_label);

private:
    const string label;

    void onCreate();
};

// Main menu scene
export class ExampleMainScene : public Node {
public:
    ~ExampleMainScene() override = default;

    void onReady() override;

//     // update the FPS displayed in the topbar
//     inline void onProcess(float alpha) override { topbar->updateFPS(); }
//
//     // build the main menu and the top bar (displayed in the examples)
//     void onEnterScene() override;
//
// private:
//     shared_ptr<GWindow> menu;
//     shared_ptr<TopBar>  topbar;
//     shared_ptr<Node>    scene;
//
//     void onMenu();
//
//     void onMenuQuit();
//
//     void onMenuTriangle() const;
//
//     void onMenuAddRemoveChild() const;
//
//     void onMenuRaycast() const;
//
//     void onMenuTerrain() const;
};
