#include "includes.h"
#include "player.h"
#include "crate.h"
#include "layers.h"
#include "topbar.h"
#include "physics.h"

void PhysicsMainScene::onReady() {
    if (parent == nullptr) {
        addChild(make_shared<Skybox>("res/textures/sky", ".jpg"));
    }
    addChild(make_shared<Environment>(vec4{1.0,1.0,1.0,0.25f}));

    setProcessMode(PROCESS_MODE_ALWAYS);

    auto game = make_shared<Node>("Game");
    game->setProcessMode(PROCESS_MODE_PAUSABLE);
    addChild(game);

    auto directionalLight1 = make_shared<DirectionalLight>(
        vec3{-1.0f, -1.0f, -1.0f},
        vec4{1.0f, 1.0f, 1.0f, 1.0f}
    );
    directionalLight1->setCastShadow(true);
    game->addChild(directionalLight1);

    auto omniLight1 = make_shared<OmniLight>(0.14, 0.07);
    omniLight1->setPosition({-6.0f, 0.5f, -8.0f});
    omniLight1->setColorAndIntensity({0.0f, 1.0f, .0f, 2.0f});
    game->addChild(omniLight1);
    
    auto spotLight1 = make_shared<SpotLight>(
        vec3{-0.8f,-0.5f, -1.0f},
        25.0f, 35.0f,
        0.027, 0.0028
    );
    spotLight1->setPosition({14.0, 3.0, 0.0});
    spotLight1->setColorAndIntensity({1.0f, 0.0f, 0.0f, 2.0f});
    spotLight1->setCastShadow(true);
    game->addChild(spotLight1);

    player = make_shared<Player>();
    game->addChild(player);
    player->setPosition({0.0, 2.0, 0.0});

    raycast = make_shared<RayCast>(vec3{0.0f, 0.0f, -100.0f}, Layers::BODIES);
    player->addChild(raycast);

    auto crateModel = Loader::loadModelFromFile("res/models/crate.glb", true);
    for (int x = 0; x < 5; x++) {
        for (int z = 0; z < 5; z++) {
            auto model = make_shared<Crate>(crateModel->duplicate());
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
            0,
            "Floor");
    floor->addChild(floorModel);
    floor->setPosition({0.0, -2.0, 0.0});
    game->addChild(floor);
    //printTree();

    player->connect("pushpull", this, reinterpret_cast<Signal::Handler>(&onPushOrPull));
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
            infoBox->setX((VECTOR_SCALE.x - width) / 2);
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
    menu = make_shared<GWindow>(Rect{0, 900, 130, 45});
    app().addWindow(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setFont(make_shared<Font>(menu->getWidget().getFont()->getFontName(),
                                                menu->getWidget().getFont()->getFontSize() / 2));
    menu->getWidget().setTransparency(0.2f);
    menu->getWidget().add(make_shared<GText>("[SPACE] Jump"), GWidget::TOP);
    menu->getWidget().add(make_shared<GText>("[ESC] Toggle mouse"), GWidget::TOP);

    infoBox = make_shared<GWindow>(Rect{0, 800, 300, 45});
    infoBox->hide();
    app().addWindow(infoBox);
    infoText = make_shared<GText>("Info");
    infoText->setTextColor({0.5f, 0.5f, 0.0f, 1.0f});
    infoBox->getWidget().add(infoText, GWidget::TOPCENTER);
    actionsText = make_shared<GText>("[P][RB] : Push   [O][LB] : Pull");
    actionsText->setTextColor({0.5f, 0.5f, 0.5f, 1.0f});
    infoBox->getWidget().add(actionsText, GWidget::TOPCENTER);
    infoBox->getWidget().setTransparency(0.2);
    infoBox->getWidget().setPadding(5);
}

void PhysicsMainScene::onExitScene() {
    app().removeWindow(menu);
}