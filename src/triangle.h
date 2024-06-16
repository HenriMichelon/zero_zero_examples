#pragma once

class TriangleMainScene: public Node, public GEventHandler {
public:
    TriangleMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onEnterScene() override;
    void onExitScene() override;
    void onPhysicsProcess(float delta) override;
    void onProcess(float alpha) override;
    bool onInput(InputEvent& inputEvent) override;

private:
    shared_ptr<MeshInstance> triangle1;
    shared_ptr<MeshInstance> triangle2;
    shared_ptr<StandardMaterial> material1;
    shared_ptr<ShaderMaterial> material2;
    float gradientSpeed{0.5f};
    float gradient{0.0f};
    bool rotate{true};
    shared_ptr<GWindow> menu;

    void onMenuRotate(GWidget*w=nullptr, GEvent*e=nullptr);
    void onMenuShader(GWidget*w=nullptr, GEvent*e=nullptr);
};
