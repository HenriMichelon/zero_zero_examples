module;
#include "libraries.h"

module Example;

import :TriangleMainScene;

TriangleMainScene::TriangleMainScene():
    Node{"Main Scene"} {
}

void TriangleMainScene::onReady() {
    // Replace the menu's camera with the scene's camera
    const auto camera = make_shared<Camera>();
    camera->setPosition({0.0f, 0.0f, 2.0f});
    addChild(camera);
    Application::get().activateCamera(camera);

    // Our triangles are constructed by code
    const vector<Vertex> vertices{
            {.position = {0.0, 0.5, 0.0}, .uv = {0.5, 0.25}},
            {.position = {0.5, -0.5, 0.0}, .uv = {0.75, 0.75}},
            {.position = {-0.5, -0.5, 0.0f}, .uv = {0.25, 0.75}},
    };
    const vector<uint32_t> indices{
            0,
            1,
            2
    };

    // Index-based surface for the first triangle
    const vector surfaces1{
            make_shared<Surface>(0, indices.size())
    };
    // Mesh for the first triangle
    auto mesh1 = Mesh::create(vertices, indices, surfaces1);
    // Standard material for the first triangle
    // With only a color and alpha transparency enabled
    material1 = make_shared<StandardMaterial>();
    material1->setAlbedoColor({0.75, 0.75, 0.75, 0.75});
    material1->setTransparency(Transparency::ALPHA);
    material1->setCullMode(CullMode::DISABLED);
    // We apply the material to the unique surface
    mesh1->setSurfaceMaterial(0, material1);
    // Create, place and add the Node to the scene
    triangle1 = make_shared<MeshInstance>(mesh1, "Triangle 1");
    triangle1->setPosition({1.0, 0.0, 0.0});
    addChild(triangle1);

    // Index-based surface for the second triangle
    const vector surfaces2{
            make_shared<Surface>(0, indices.size())
    };
    // Mesh for the second triangle
    auto mesh2 = Mesh::create(vertices, indices, surfaces2);
    // Shader based material for the second triangle
    // With a fragment shader, a vertex shader and alpha transparency enabled
    material2 = make_shared<ShaderMaterial>(
            "examples/uv_gradient.frag",
            "examples/scale.vert");
    material2->setTransparency(Transparency::ALPHA);
    material2->setCullMode(CullMode::DISABLED);
    material2->setParameter(0, vec4{0.0f}); // parameter for the fragment shader
    material2->setParameter(1, vec4{0.0f}); // parameter for the vertex shader
    // We apply the material to the unique surface
    mesh2->setSurfaceMaterial(0, material2);
    // Create, place and add the Node to the scene
    triangle2 = make_shared<MeshInstance>(mesh2, "Triangle 2");
    triangle2->setPosition({-1.0, 0.0, 0.0});
    addChild(triangle2);
}

void TriangleMainScene::onEnterScene() {
    // Build and display the scene menu
    menu = make_shared<GWindow>(Rect{0, 1000 - 550, 150, 500});
    Application::get().add(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setDrawBackground(false);

    // Toggle rotation button
    const auto menuRotate = make_shared<GButton>();
    menu->getWidget().add(menuRotate, GWidget::TOPCENTER, "200,40");
    menuRotate->add(make_shared<GText>("[SPACE] Toggle rotation"), GWidget::CENTER);
    menuRotate->connect(GEvent::OnClick, [this]{this->onMenuRotate();});

    // Toggle right triangle material button
    const auto menuShader = make_shared<GButton>();
    menu->getWidget().add(menuShader, GWidget::TOPCENTER, "200,40");
    menuShader->add(make_shared<GText>("[ENTER] Toggle Shader"), GWidget::CENTER);
    menuShader->connect(GEvent::OnClick, [this]{this->onMenuShader();});
}

void TriangleMainScene::onExitScene() {
    // Remove the scene menu before returning to the main menu
    Application::get().remove(menu);
}

void TriangleMainScene::onPhysicsProcess(const float delta) {
    // If rotation is enabled rotate the triangles
    if (rotate) {
        const auto angle = delta * radians(90.0f) / 2;
        triangle1->rotateY(angle);
        triangle2->rotateY(-angle);
    }
    // Rotate the color gradient used by the fragment shader
    gradient += gradientSpeed * delta;
    // Ensure the color gradient remains within the range [0, 1]
    gradient = std::clamp(gradient, 0.0f, 1.0f);
    if (gradient == 1.0f || gradient == 0.0f) {
        gradientSpeed = -gradientSpeed;
    }
    // Send the parameters to the shaders
    // We use the grandient value for the triangle colors
    // AND for the triangle deformation scale
    material2->setParameter(0, vec4{gradient});
    material2->setParameter(1, vec4{gradient});
}

// input example with the Input singleton
void TriangleMainScene::onProcess(const float alpha) {
    if (Input::isKeyJustPressed(KEY_ENTER)) {
        onMenuShader();
    }
}

// Input example with the input event system
bool TriangleMainScene::onInput(InputEvent &inputEvent) {
    if (inputEvent.getType() == InputEventType::KEY) {
        const auto &eventKey = dynamic_cast<InputEventKey &>(inputEvent);
        if ((eventKey.getKey() == KEY_SPACE) && !eventKey.isPressed()) {
            onMenuRotate();
            return true;
        }
    }
    return false;
}

void TriangleMainScene::onMenuRotate() {
    // stop or restart rotation
    rotate = !rotate;
}

void TriangleMainScene::onMenuShader() const {
    // toggle material of the right triangle
    if (triangle1->getMesh()->getSurfaceMaterial(0).get() == material1.get()) {
        triangle1->getMesh()->setSurfaceMaterial(0, material2);
    } else {
        triangle1->getMesh()->setSurfaceMaterial(0, material1);
    }
}
