#include "includes.h"
#include "layers.h"
#include "player.h"
#include "terrain.h"

void TerrainScene::onReady() {
    setProcessMode(PROCESS_MODE_ALWAYS);
    auto game = make_shared<Node>("Game");
    game->setProcessMode(PROCESS_MODE_PAUSABLE);
    addChild(game);

    auto directionalLight = make_shared<DirectionalLight>(
        vec3{-0.2f, -0.8f, 1.0f},
        vec4{1.0f, 1.0f, 1.0f, 1.0f}
    );
    directionalLight->setCastShadow(true);
    game->addChild(directionalLight);

    player = make_shared<Player>(10);
    player->setPosition({10.0f, 10.0f, 5.0f});
    game->addChild(player);

    auto terrainModel = Loader::loadModelFromFile("res/models/terrain.glb", true);
    vector<SubShape> terrainCollisionShapes;
    terrainCollisionShapes.push_back(SubShape{make_shared<MeshShape>(terrainModel)});
    // virtual walls
    terrainCollisionShapes.push_back(SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, -100.0}});
    terrainCollisionShapes.push_back(SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, 100.0}});
    terrainCollisionShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{100.0, 5.0, 0.0}});
    terrainCollisionShapes.push_back(SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{-100.0, 5.0, 0.0}});

    auto terrain = make_shared<StaticBody>(
            make_shared<StaticCompoundShape>(terrainCollisionShapes),
            Layers::WORLD,
            0,
            "Terrain");
    terrain->addChild(terrainModel);
    terrain->setPosition({0.0, -1.0, 0.0});
    game->addChild(terrain);
}
