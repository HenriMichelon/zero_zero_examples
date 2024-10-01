module;
#include <simdjson.h>

#include "libraries.h"

export module Example:PhysicsMainScene;

import :Player;
import :Crate;
import :Layers;

export class PhysicsMainScene : public Node {
public:
    PhysicsMainScene():
        Node{"Main Scene"} {
    };

    void onReady() override {
        // make the scene node not pauseable
        setProcessMode(PROCESS_MODE_ALWAYS);
        // add the global environement
        addChild(make_shared<Environment>(vec4{1.0, 1.0, 1.0, 0.6f}));

        // add a game node and make it pausable since the scene can't be paused
        auto game = make_shared<Node>("Game");
        game->setProcessMode(PROCESS_MODE_PAUSABLE);
        addChild(game);

        // add the Sun
        auto directionalLight1 = make_shared<DirectionalLight>(
                vec3{0.0f, -0.25f, 1.0f},
                vec4{1.0f, 1.0f, 1.0f, 0.5f}
                );
        directionalLight1->setCastShadow(true);
        game->addChild(directionalLight1);

        // add the player
        player = make_shared<Player>();
        game->addChild(player);

        // add an optional spot light
        auto spotLight1 = make_shared<SpotLight>(
                vec3{0.0f, -0.25f, -1.0f},
                10.0f,
                12.0f,
                0.022,
                0.0019
                );
        spotLight1->setPosition({0.0, 1.0, -0.1});
        spotLight1->setColorAndIntensity({1.0f, 1.0f, 0.0f, 2.0f});
        //spotLight1->setCastShadow(true);
        player->addChild(spotLight1);

        // raycast used to detect crates in front of the palyer
        raycast = make_shared<RayCast>(vec3{0.0f, 0.0f, -100.0f}, BODIES);
        player->addChild(raycast);

        // generates crates nodes with random positions
        const auto crateScene = Loader::loadModelFromFile("res/models/crate.glb", true);
        const auto& crateModel = crateScene->getChild("Crate");
        for (int x = 0; x < 5; x++) {
            for (int z = 0; z < 5; z++) {
                auto body = make_shared<Crate>();
                auto model = crateModel->duplicate();
                body->addChild(model);
                body->setPosition({x * 5 - 1.5 * 5, 1.0 + rand() % 5, -z * 5 - 5});
                game->addChild(body);
            }
        }

        // create the material to outline the crates in front of the player
        raycastOutlineMaterial = make_shared<ShaderMaterial>(OutlineMaterials::get(0));
        raycastOutlineMaterial->setParameter(0, {1.0, 0.0, 0.0, 1.0});
        raycastOutlineMaterial->setParameter(1, vec4{0.005});
        OutlineMaterials::add(raycastOutlineMaterial);

        // create material to outline the crate in collision with the player
        collisionOutlineMaterial = make_shared<ShaderMaterial>(OutlineMaterials::get(0));
        collisionOutlineMaterial->setParameter(0, {0.0, 1.0, 0.0, 1.0});
        collisionOutlineMaterial->setParameter(1, vec4{0.02});
        OutlineMaterials::add(collisionOutlineMaterial);

        // build the scene floor node and associated static body
        const auto floorScene = Loader::loadModelFromFile("res/models/floor.glb", true);
        const auto floorModel = floorScene->getChild("Cube");
        floorScene->removeChild(floorModel);
        vector<SubShape> floorSubShapes;
        floorSubShapes.push_back(SubShape{make_shared<ConvexHullShape>(floorModel)});
        // add virtual walls
        floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, -100.0}});
        floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, 100.0}});
        floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{100.0, 5.0, 0.0}});
        floorSubShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{-100.0, 5.0, 0.0}});
        // the static body to make the floor collides with the player and the crates
        auto floor = make_shared<StaticBody>(
                make_shared<StaticCompoundShape>(floorSubShapes),
                WORLD,
                "Floor");
        floor->addChild(floorModel);
        floor->setPosition({0.0, -1.0, 0.0});
        game->addChild(floor);
        printTree();

        // connect the player signals for the "push" and "pull" actions
        player->connect(Player::on_push_pull, this, Signal::Handler(&PhysicsMainScene::onPushOrPull));
    }

    void onProcess(float alpha) override {
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
            auto *      meshInstance = collider.findFirstChild<MeshInstance>();
            // if not already outlined activate and set the outline material
            if (!meshInstance->isOutlined()) {
                meshInstance->setOutlined(true);
                meshInstance->setOutlineMaterial(raycastOutlineMaterial);
                previousSelection = meshInstance;
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
                auto *meshInstance = collision.object->findFirstChild<MeshInstance>();
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

    void onEnterScene() override {
        // add the scene menu
        menu = make_shared<GWindow>(Rect{0, 850, 130, 100});
        app().add(menu);
        menu->getWidget().setPadding(5);
        menu->getWidget().setTransparency(0.2f);

        // buttons of the scene menu
        auto textJump  = make_shared<GText>("[SPACE] Jump");
        auto textMouse = make_shared<GText>("[ESC] Toggle mouse");
        menu->getWidget().add(textJump, GWidget::TOP);
        menu->getWidget().add(textMouse, GWidget::TOP);
        menu->setHeight(textJump->getHeight() + textMouse->getHeight() + menu->getWidget().getPadding() * 3);

        // build the information box dispalyed when we collide a crate
        infoBox = make_shared<GWindow>(Rect{0, 800, 200, 100});
        infoBox->hide();
        app().add(infoBox);
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

    void onExitScene() override {
        // remove the scene menu and info box before returning to the main menu
        app().remove(menu);
        app().remove(infoBox);
    }

private:
    // force used when pulling or pushing a crate
    const vec3 force = vec3{7500.0f};

    // scene menu
    shared_ptr<GWindow> menu;
    // ou player node
    shared_ptr<Player>  player;
    // raycast used to outline the crate in fron of the player
    shared_ptr<RayCast>        raycast;
    // material for the selection outline
    shared_ptr<ShaderMaterial> raycastOutlineMaterial;
    // used to save the currently selected crate to disable the outline
    MeshInstance *             previousSelection{nullptr};
    // information box used when the player collide with a crate
    shared_ptr<GWindow>              infoBox;
    // text displaying the colliding object name
    shared_ptr<GText>                infoText;
    // text displaying the actions the player can do when colliding a crate
    shared_ptr<GText>                actionsText;
    // outline material for colliding crates
    shared_ptr<ShaderMaterial>       collisionOutlineMaterial;
    // list a colliding crate to reset the outline materials
    list<CollisionObject::Collision> currentCollisions;

    // player actions : push or pull a crate
    bool pushing{false};
    bool pulling{false};

    // signal handler called on a player action
    void onPushOrPull(Player::PushOrPullAction *action) {
        pushing = action->push;
        pulling = action->pull;
    }
};
