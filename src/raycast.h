#pragma once

class RaycastMainScene: public Node, public GEventHandler {
public:
    RaycastMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onProcess(float alpha) override;
private:
    shared_ptr<z0::RayCast> raycast;
    shared_ptr<TopBar> topbar;
    void onMenuQuit(GWidget&, GEvent*);
};
