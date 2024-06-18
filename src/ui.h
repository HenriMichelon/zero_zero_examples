#pragma once

class Window2 : public GWindow {
public:
    explicit Window2(Rect rect) : GWindow{rect} {}
    void onCreate() override;
private:
    void onButtonClic(GEventClick*);
};

class UIMainScene: public Node {
public:
    UIMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onPhysicsProcess(float delta) override;
    bool onInput(z0::InputEvent &inputEvent) override;
private:
    shared_ptr<Node> sphere;
    shared_ptr<Window2> window2;
};
