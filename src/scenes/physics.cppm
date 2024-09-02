module;
#include <z0/z0.h>
using namespace z0;
#include "layers.h"

export module Example:PhysicsMainScene;

import :Player;
import :Crate;

export class PhysicsMainScene: public Node {
public:
    PhysicsMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    void onProcess(float alpha) override;
    void onEnterScene() override;
    void onExitScene() override;
private:
    const vec3  force = vec3{7500.0f};

    shared_ptr<GWindow> menu;
    shared_ptr<Player> player;

    shared_ptr<RayCast> raycast;
    shared_ptr<ShaderMaterial> raycastOutlineMaterial;
    MeshInstance* previousSelection{nullptr};
    
    shared_ptr<GWindow> infoBox;
    shared_ptr<GText> infoText;
    shared_ptr<GText> actionsText;
    shared_ptr<ShaderMaterial> collisionOutlineMaterial;
    list<CollisionObject::Collision> currentCollisions;

    bool pushing{false};
    bool pulling{false};
    void onPushOrPull(Player::PushOrPullAction* action);
};

void PhysicsMainScene::onReady() {
    addChild(make_shared<Environment>(vec4{1.0,1.0,1.0,0.6f}));

    setProcessMode(PROCESS_MODE_ALWAYS);

    auto game = make_shared<Node>("Game");
    game->setProcessMode(PROCESS_MODE_PAUSABLE);
    addChild(game);

    auto directionalLight1 = make_shared<DirectionalLight>(
        vec3{0.0f, -0.25f, 1.0f},
        vec4{1.0f, 1.0f, 1.0f, 0.5f}
    );
    directionalLight1->setCastShadow(true);
    game->addChild(directionalLight1);

    player = make_shared<Player>();
    game->addChild(player);

    auto spotLight1 = make_shared<SpotLight>(
        vec3{0.0f, -0.25f, -1.0f},
        10.0f, 12.0f,
        0.022, 0.0019
    );
    spotLight1->setPosition({0.0,1.0,-0.1});
    spotLight1->setColorAndIntensity({1.0f, 1.0f, 0.0f, 2.0f});
    //spotLight1->setCastShadow(true);
    player->addChild(spotLight1);

    raycast = make_shared<RayCast>(vec3{0.0f, 0.0f, -100.0f}, Layers::BODIES);
    player->addChild(raycast);

    auto crateModel = Loader::loadModelFromFile("res/models/crate.glb", true);
    for (int x = 0; x < 5; x++) {
        for (int z = 0; z < 5; z++) {
            auto model = make_shared<Crate>();
            model->addChild(crateModel->duplicate());
            model->setPosition({x * 5 - 1.5 * 5, 1.0 + rand() % 5, -z * 5 - 5});
            game->addChild(model);
        }
    }

    raycastOutlineMaterial = make_shared<ShaderMaterial>(OutlineMaterials::get(0));
    raycastOutlineMaterial->setParameter(0, {1.0,0.0,0.0,1.0});
    raycastOutlineMaterial->setParameter(1, vec4{0.005});
    OutlineMaterials::add(raycastOutlineMaterial);
    collisionOutlineMaterial = make_shared<ShaderMaterial>(OutlineMaterials::get(0));
    collisionOutlineMaterial->setParameter(0, {0.0,1.0,0.0,1.0});
    collisionOutlineMaterial->setParameter(1, vec4{0.02});
    OutlineMaterials::add(collisionOutlineMaterial);

    auto floorModel = Loader::loadModelFromFile("res/models/floor.glb", true);

    vector<SubShape> floorSubShapes;
    floorSubShapes.push_back(SubShape{make_shared<ConvexHullShape>(floorModel)});
    // virtual walls
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, -100.0}});
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, 100.0}});
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{100.0, 5.0, 0.0}});
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{-100.0, 5.0, 0.0}});

    auto floor = make_shared<StaticBody>(
            make_shared<StaticCompoundShape>(floorSubShapes),
            Layers::WORLD,
            "Floor");
    floor->addChild(floorModel);
    floor->setPosition({0.0, -1.0, 0.0});
    game->addChild(floor);
    //printTree();

    player->connect(Player::on_push_pull, this, Signal::Handler(&PhysicsMainScene::onPushOrPull));
}

void PhysicsMainScene::onProcess(float alpha) {
    if (previousSelection != nullptr) {
        if (previousSelection->getOutlineMaterial() == raycastOutlineMaterial) {
            previousSelection->setOutlined(false);
        }
        previousSelection = nullptr;
    }
    if (raycast->isColliding()) {
        const auto& collider = *(raycast->getCollider());
        auto* meshInstance = collider.findFirstChild<MeshInstance>();
        if (!meshInstance->isOutlined()) {
            meshInstance->setOutlined(true);
            meshInstance->setOutlineMaterial(raycastOutlineMaterial);
            previousSelection = meshInstance;
        }
    }
    for (const auto& collision: currentCollisions) {
         collision.object->findFirstChild<MeshInstance>()->setOutlined(false);
    }
    currentCollisions.clear();
    for(const auto& collision : player->getCollisions()) {
        if ((!player->isGround(collision.object) &&
            (collision.normal.y < 0.8))) { // do not select when on top of a crate
            if (pushing || pulling) {
                collision.object->applyForce(
                    force * collision.normal * (pushing ? -1.0f : 1.0f),
                    collision.position);
            }
            auto* meshInstance = collision.object->findFirstChild<MeshInstance>();
            meshInstance->setOutlined(true);
            meshInstance->setOutlineMaterial(collisionOutlineMaterial);
            currentCollisions.push_back(collision);
        }
    }
    if (!currentCollisions.empty()) {
        if (!infoBox->isVisible()) {
            infoText->setText(currentCollisions.front().object->toString());
            auto width = std::max(infoText->getWidth(), actionsText->getWidth());
            infoBox->setWidth(width + infoBox->getWidget().getPadding() * 2);
            infoBox->setX((VECTOR_SCALE.x - infoBox->getWidth()) / 2);
            infoBox->show();
        }
    } else if (infoBox->isVisible()) {
        infoBox->hide();
    }
}

void PhysicsMainScene::onPushOrPull(Player::PushOrPullAction* action) {
    pushing = action->push;
    pulling = action->pull;
}

void PhysicsMainScene::onEnterScene() {
    menu = make_shared<GWindow>(Rect{0, 850, 130, 45});
    app().addWindow(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setTransparency(0.2f);
    auto textJump = make_shared<GText>("[SPACE] Jump");
    auto textMouse = make_shared<GText>("[ESC] Toggle mouse");
    menu->getWidget().add(textJump, GWidget::TOP);
    menu->getWidget().add(textMouse, GWidget::TOP);
    menu->setHeight(textJump->getHeight() + textMouse->getHeight() + menu->getWidget().getPadding() * 3);

    infoBox = make_shared<GWindow>(Rect{0, 800, 10, 10});
    infoBox->hide();
    app().addWindow(infoBox);
    infoText = make_shared<GText>("Info");
    infoText->setTextColor({0.8f, 0.2f, 0.2f, 1.0f});
    infoBox->getWidget().add(infoText, GWidget::TOPCENTER);
    actionsText = make_shared<GText>("[P][RB] : Push   [O][LB] : Pull");
    actionsText->setTextColor({0.2f, 0.2f, 0.8f, 1.0f});
    infoBox->getWidget().add(actionsText, GWidget::TOPCENTER);
    infoBox->getWidget().setTransparency(0.8);
    infoBox->getWidget().setPadding(5);
    infoBox->setHeight(infoText->getHeight() + actionsText->getHeight() + infoBox->getWidget().getPadding() * 3);
}

void PhysicsMainScene::onExitScene() {
    app().removeWindow(menu);
    app().removeWindow(infoBox);
}