module;
#include "libraries.h"

export module Example:TriangleMainScene;

export class TriangleMainScene : public Node {
public:
    TriangleMainScene(): Node{"Main Scene"} {
    };

    void onReady() override {
        const auto camera = make_shared<Camera>();
        camera->setPosition({0.0f, 0.0f, 2.0f});
        addChild(camera);
        app().activateCamera(camera);

        const vector<Vertex> vertices{
            {.position = {0.0, 0.5, 0.0}, .uv = {0.5, 0.25}},
            {.position = {0.5, -0.5, 0.0}, .uv = {0.75, 0.75}},
            {.position = {-0.5, -0.5, 0.0f}, .uv = {0.25, 0.75}},
        };
        const vector<uint32_t> indices{
            0, 1, 2
        };

        const vector<shared_ptr<Surface>> surfaces1{
            make_shared<Surface>(0, indices.size())
        };
        auto mesh1 = make_shared<Mesh>(vertices, indices, surfaces1);
        material1 = make_shared<StandardMaterial>();
        material1->setAlbedoColor(Color(vec4{0.5, 0.5, 0.5, 0.5}));
        material1->setTransparency(TRANSPARENCY_ALPHA);
        material1->setCullMode(CULLMODE_DISABLED);
        mesh1->setSurfaceMaterial(0, material1);
        triangle1 = make_shared<MeshInstance>(mesh1);
        triangle1->setPosition({1.0, 0.0, 0.0});
        addChild(triangle1);

        const vector<shared_ptr<Surface>> surfaces2{
            make_shared<Surface>(0, indices.size())
        };
        auto mesh2 = make_shared<Mesh>(vertices, indices, surfaces2);
        material2 = make_shared<ShaderMaterial>("examples/uv_gradient.frag", "examples/scale.vert");
        material2->setTransparency(TRANSPARENCY_ALPHA);
        material2->setCullMode(CULLMODE_DISABLED);
        material2->setParameter(0, vec4{0.0f});
        material2->setParameter(1, vec4{0.0f});
        mesh2->setSurfaceMaterial(0, material2);
        triangle2 = make_shared<MeshInstance>(mesh2);
        triangle2->setPosition({-1.0, 0.0, 0.0});
        addChild(triangle2);
    }

    void onEnterScene() override {
        menu = make_shared<GWindow>(Rect{0, 1000 - 550, 150, 500});
        app().addWindow(menu);
        menu->getWidget().setPadding(5);
        menu->getWidget().setDrawBackground(false);

        const auto menuRotate = make_shared<GButton>();
        menu->getWidget().add(menuRotate, GWidget::TOPCENTER, "200,40");
        menuRotate->add(make_shared<GText>("[SPACE] Toggle rotation"), GWidget::CENTER);
        menuRotate->connect(GEvent::OnClick, this, Signal::Handler(&TriangleMainScene::onMenuRotate));
        const auto menuShader = make_shared<GButton>();
        menu->getWidget().add(menuShader, GWidget::TOPCENTER, "200,40");
        menuShader->add(make_shared<GText>("[ENTER] Toggle Shader"), GWidget::CENTER);
        menuShader->connect(GEvent::OnClick, this, Signal::Handler(&TriangleMainScene::onMenuShader));
    }

    void onExitScene() override {
        app().removeWindow(menu);
    }

    void onPhysicsProcess(float delta) override {
        if (rotate) {
            const auto angle = delta * radians(90.0f) / 2;
            triangle1->rotateY(angle);
            triangle2->rotateY(-angle);
        }
        gradient += gradientSpeed * delta;
        // Ensure the color component remains within the range [0, 1]
        if (gradient > 1.0f) {
            gradient = 1.0f;
            gradientSpeed = -gradientSpeed;
        }
        if (gradient < 0.0f) {
            gradient = 0.0f;
            gradientSpeed = -gradientSpeed;
        }
        material2->setParameter(0, vec4{gradient});
        material2->setParameter(1, vec4{gradient});
    }

    void onProcess(float alpha) override {
        if (Input::isKeyJustPressed(KEY_ENTER)) {
            onMenuShader();
        }
    }

    bool onInput(InputEvent& inputEvent) override {
        if (inputEvent.getType() == INPUT_EVENT_KEY) {
            auto& eventKey = dynamic_cast<InputEventKey&>(inputEvent);
            if ((eventKey.getKey() == KEY_SPACE) && !eventKey.isPressed()) {
                onMenuRotate();
                return true;
            }
        }
        return false;
    }

private:
    shared_ptr<MeshInstance> triangle1;
    shared_ptr<MeshInstance> triangle2;
    shared_ptr<StandardMaterial> material1;
    shared_ptr<ShaderMaterial> material2;
    float gradientSpeed{0.5f};
    float gradient{0.0f};
    bool rotate{true};
    shared_ptr<GWindow> menu;

    void onMenuRotate(GEventClick* e = nullptr) {
        rotate = !rotate;
    }

    void onMenuShader(GEventClick* e = nullptr) {
        if (triangle1->getMesh()->getSurfaceMaterial(0).get() == material1.get()) {
            triangle1->getMesh()->setSurfaceMaterial(0, material2);
        }
        else {
            triangle1->getMesh()->setSurfaceMaterial(0, material1);
        }
    }
};