#pragma once

class TopBar: public GWindow {
public:
    TopBar(Object* obj, Signal::Handler onQuit);
    void onCreate() override;
    void updateFPS();

private:
    uint32_t fps{0};
    shared_ptr<GText> textFPS;
    Object* onQuitHandler;
    Signal::Handler onQuit;
    void onPauseToggle(GEventClick*);
};