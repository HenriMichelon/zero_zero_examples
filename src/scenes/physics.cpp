module;
#include <glm/gtx/quaternion.hpp>
#include "libraries.h"

module Example;

import :PhysicsMainScene;
import :Player;
import :Crate;
import :Layers;

PhysicsMainScene::PhysicsMainScene():
    Node{"Main Scene"} {
}

void PhysicsMainScene::onReady() {
    // make the scene node not pauseable
    setProcessMode(ProcessMode::ALWAYS);
    // add the global environment
    addChild(make_shared<Environment>(vec4{1.0, 1.0, 1.0, 0.1f}));

    // add a game node and make it pauseable since the scene can't be paused
    const auto game = make_shared<Node>("Game");
    game->setProcessMode(ProcessMode::PAUSABLE);
    addChild(game);

    // add the Sun
    const auto directionalLight1 = make_shared<DirectionalLight>(vec4{1.0f, 1.0f, 1.0f, 0.8f});
    directionalLight1->setRotation(radians(vec3{-45.0f, 138.0f, 0.0f}));
    directionalLight1->setCastShadows(true);
    game->addChild(directionalLight1);

    // add the player
    player = make_shared<Player>();
    player->setPosition(vec3{0.0f, 0.0f, 10.0f});
    game->addChild(player);

    // add an optional spotlight
    const auto spotLight1 = make_shared<SpotLight>(
            8.0f, 12.0f, 10.0f,
            vec4{1.0f, 1.0f, 0.0f, 2.0f});
    spotLight1->setPosition({0.0, 1.0, -0.5});
    spotLight1->rotateX(radians(-20.0f));
    // player->addChild(spotLight1);

    // raycast used to detect crates in front of the player
    raycast = make_shared<RayCast>(AXIS_FRONT * 50.0f, BODIES);
    player->addChild(raycast);

    // generates crates nodes with random positions
    const auto &crateScene = Loader::load("app://res/models/crate.zscene");
    const auto &crateModel = crateScene->getChild("Crate");
    for (int x = 0; x < 4; x++) {
        for (int z = 0; z < 4; z++) {
            const auto model = crateModel->duplicate();
            auto body  = make_shared<Crate>();
            body->addChild(model);
            body->setPosition({x * 5 - 1.5 * 5, 1.0 + rand() % 5, -z * 5 + 5});
            game->addChild(body);
        }
    }

    // create the material to outline the crates in front of the player
    auto& outlineMaterials = Application::get().getOutlineMaterials();
    raycastOutlineMaterial = make_shared<ShaderMaterial>(outlineMaterials.get(0));
    raycastOutlineMaterial->setParameter(0, {1.0, 0.0, 0.0, 1.0});
    raycastOutlineMaterial->setParameter(1, vec4{0.005});
    outlineMaterials.add(raycastOutlineMaterial);

    // create material to outline the crate in collision with the player
    collisionOutlineMaterial = make_shared<ShaderMaterial>(outlineMaterials.get(0));
    collisionOutlineMaterial->setParameter(0, {0.0, 1.0, 0.0, 1.0});
    collisionOutlineMaterial->setParameter(1, vec4{0.02});
    outlineMaterials.add(collisionOutlineMaterial);

    // build the scene floor node and associated static body
    const auto& floorScene = Loader::load("app://res/models/playground.zscene");
    auto floorModel = floorScene->findFirstChild("Box001_asphalt_0");
    if (floorModel == nullptr) die("Floor not found");
    vector<SubShape> floorSubShapes;
    floorSubShapes.push_back(SubShape{make_shared<ConvexHullShape>(floorModel)});
    // add virtual walls
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{25.0, 10.0, 1.0}), vec3{0.0, 5.0, -15.0}});
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{25.0, 10.0, 1.0}), vec3{0.0, 5.0, 15.0}});
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 30.0}), vec3{12.5, 5.0, 0.0}});
    floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 30.0}), vec3{-12.5, 5.0, 0.0}});
    // the static body to make the floor collides with the player and the crates
    const auto floor = make_shared<StaticBody>(
            make_shared<StaticCompoundShape>(floorSubShapes),
            WORLD,
            "Floor");
    floor->addChild(floorScene);
    // floor->setPosition({0.0f, 5.0f, 0.0f});
    game->addChild(floor);

    // connect the player signals for the "push" and "pull" actions
    player->connect(Player::on_push_pull, [this](Signal::Parameters*p){this->onPushOrPull((Player::PushOrPullAction *)p);});
}

void PhysicsMainScene::onProcess(float alpha) {
    // if a crate is already selected (and optionally outlined) from the previous frame
    // deselect it for the current frame
    if (previousSelection != nullptr) {
        if (previousSelection->getOutlineMaterial() == raycastOutlineMaterial) {
            previousSelection->setOutlined(false);
        }
        previousSelection = nullptr;
    }
    // detect if a crate is in front on the player
    if (raycast->isColliding()) {
        const auto &collider     = *(raycast->getCollider());
        const auto&       meshInstance = collider.findFirstChild<MeshInstance>();
        // if not already outlined activate and set the outline material
        if (!meshInstance->isOutlined()) {
            meshInstance->setOutlined(true);
            meshInstance->setOutlineMaterial(raycastOutlineMaterial);
            previousSelection = meshInstance;
            //log("Collide ", meshInstance->toString(), " ",  to_string(meshInstance->getId()));
        }
    }
    // clear all the previously colliding crates
    // and disable the oulines off all colliding crates
    for (const auto &collision : currentCollisions) {
        collision.object->findFirstChild<MeshInstance>()->setOutlined(false);
    }
    currentCollisions.clear();
    // detect all the colliding crates
    for (const auto &collision : player->getCollisions()) {
        // only if the player is not on top of a crate
        if ((!player->isGround(collision.object) &&
            (collision.normal.y < 0.8))) {
            // push or pull the colliding crate in the colliding direction
            if (pushing || pulling) {
                collision.object->applyForce(
                        force * collision.normal * (pushing ? -1.0f : 1.0f),
                        collision.position);
            }
            // outline the colliding crate
            const auto& meshInstance = collision.object->findFirstChild<MeshInstance>();
            meshInstance->setOutlined(true);
            meshInstance->setOutlineMaterial(collisionOutlineMaterial);
            // save the colliding crate to disable the outline during the next frame
            currentCollisions.push_back(collision);
        }
    }
    // if we have collisions we display an information box for the first colliding crate
    if (!currentCollisions.empty()) {
        // only change the info box content if not already displayed
        if (!infoBox->isVisible()) {
            // name of the colliding object
            infoText->setText(currentCollisions.front().object->toString());
            const auto width = std::max(infoText->getWidth(), actionsText->getWidth());
            // resize and show the info box
            infoBox->setWidth(width + infoBox->getWidget().getPadding() * 2);
            infoBox->setX((VECTOR_SCALE.x - infoBox->getWidth()) / 2);
            infoBox->show();
        }
    } else if (infoBox->isVisible()) {
        infoBox->hide();
    }
}

void PhysicsMainScene::onEnterScene() {
    // add the scene menu
    menu = make_shared<GWindow>(Rect{0, 850, 130, 100});
    Application::get().add(menu);
    menu->getWidget().setPadding(5);
    menu->getWidget().setTransparency(0.2f);

    // buttons of the scene menu
    const auto textJump  = make_shared<GText>("[SPACE] Jump");
    const auto textMouse = make_shared<GText>("[ESC] Toggle mouse");
    menu->getWidget().add(textJump, GWidget::TOP);
    menu->getWidget().add(textMouse, GWidget::TOP);
    menu->setHeight(textJump->getHeight() + textMouse->getHeight() + menu->getWidget().getPadding() * 3);

    // build the information box dispalyed when we collide a crate
    infoBox = make_shared<GWindow>(Rect{0, 800, 200, 100});
    infoBox->hide();
    Application::get().add(infoBox);
    infoText = make_shared<GText>("Info---------------------");
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
    // remove the scene menu and info box before returning to the main menu
    Application::get().remove(menu);
    Application::get().remove(infoBox);
}

// signal handler called on a player action
void PhysicsMainScene::onPushOrPull(Player::PushOrPullAction *action) {
    pushing = action->push;
    pulling = action->pull;
}
