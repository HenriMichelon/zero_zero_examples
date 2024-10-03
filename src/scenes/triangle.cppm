module;
#include "libraries.h"

export module Example:TriangleMainScene;

using namespace z0;

// Two nice rotating triangles, look like an advanced Vulkan tutorial :)
export class TriangleMainScene : public Node {
public:
    TriangleMainScene();

    void onReady() override;

    void onEnterScene() override;

    void onExitScene() override;

    void onPhysicsProcess(const float delta) override;

    // input example with the Input singleton
    void onProcess(const float alpha) override;

    // Input example wit the input event system
    bool onInput(InputEvent &inputEvent) override;

private:
    shared_ptr<MeshInstance>     triangle1; // right triangle
    shared_ptr<MeshInstance>     triangle2; // left triangle
    shared_ptr<StandardMaterial> material1; // material for the right triangle
    shared_ptr<ShaderMaterial>   material2; // material for the left triangle
    float                        gradient{0.0f}; // color gradient and scale for the triangles, used by the shaders
    float                        gradientSpeed{0.5f}; // speed for the gradient change
    bool                         rotate{true}; // true if both the triangles rotates
    shared_ptr<GWindow>          menu; // our scene menu

    void onMenuRotate(GEventClick *e = nullptr);

    void onMenuShader(GEventClick *e = nullptr) const;
};
