#pragma once

class ExampleMainScene: public Node {
public:
    void onReady() override;
    void onProcess(float alpha) override;
    void onEnterScene() override;

private:
    shared_ptr<GWindow> menu;
    shared_ptr<TopBar> topbar;
    shared_ptr<Node> scene;

    void onMenu(GEventClick*);
    void onMenuQuit(GEventClick*);
    void onMenuTriangle(GEventClick*);
    void onMenuAddRemoveChild(GEventClick*);
    void onMenuPhysics(GEventClick*);
    void onMenuRaycast(GEventClick*);
    void onMenuTerrain(GEventClick*);
};
