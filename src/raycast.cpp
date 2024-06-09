#include "includes.h"
#include "player.h"
#include "crate.h"
#include "layers.h"
#include "topbar.h"
#include "raycast.h"

void RaycastMainScene::onReady() {
    setProcessMode(PROCESS_MODE_ALWAYS);

    auto game = make_shared<Node>("Game");
    game->setProcessMode(PROCESS_MODE_PAUSABLE);
    addChild(game);
    
    auto player = make_shared<Player>();
    game->addChild(player);
    player->setPosition({0.0, 0.5, 0.0});

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

    auto floor = make_shared<StaticBody>(
            make_shared<BoxShape>(vec3{200.0f, 0.2f, 200.0f}),
            Layers::WORLD,
            0,
            "Floor");
    floor->addChild(Loader::loadModelFromFile("res/models/floor.glb", true));
    floor->setPosition({0.0, -2.0, 0.0});
    game->addChild(floor);
    printTree();
}

void RaycastMainScene::onProcess(float alpha) {
    if (previousSelection != nullptr) {
        previousSelection->setOutlined(false);
        previousSelection = nullptr;
    }
    if (raycast->isColliding()) {
        const auto& collider = *(raycast->getCollider());
        log("Raycast hit", 
            to_string(collider.getId()), 
            collider.toString(), 
            z0::toString(raycast->getCollisionPoint()));
        auto* meshInstance = dynamic_cast<MeshInstance*>(collider.getNode("res_models_crate.glb/Sketchfab_model/Collada visual scene group/g/defaultMaterial").get());
        meshInstance->setOutlined(true);
        previousSelection = meshInstance;
    }
}
