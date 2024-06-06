#pragma once

class RaycastMainScene: public Node, public GEventHandler {
public:
    RaycastMainScene(): Node{"Main Scene"} {};
    void onReady() override;
private:
    void onMenuQuit(GWidget&, GEvent*);
};
