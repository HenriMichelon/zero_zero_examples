module;
#include "libraries.h"

export module Example:Platforms;

import :Layers;

using namespace z0;

export class PlatformsScene : public Node {
public:
    void onReady() override {
        setProcessMode(PROCESS_MODE_ALWAYS);
        log("Loading scene");
        Loader::addSceneFromFile(this, "res/scenes/scene3.json");
        const auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
        log("VRAM usage", to_string(usage) + "%");

        auto game     = getChild("game");
        auto level    = game->getNode("level1/level1Resources");
        auto mountain = level->getNode("Mountains/Mountain1");
        createBodies(*game, *mountain);
        auto spawnPoint = level->getNode("SpawnPoint");
        auto player = game->getChild("player");
        player->setPositionGlobal(spawnPoint->getPositionGlobal());
        printTree();
    }

    ~PlatformsScene() override = default;

private:
    void createBodies(Node &game, const Node &node) {
        for (const auto &child : node.getChildren()) {
            game.addChild(make_shared<StaticBody>(make_shared<ConvexHullShape>(child), WORLD));
        }
    }
};
