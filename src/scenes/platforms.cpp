module;
#include "libraries.h"

module Example;
import :Layers;
import :PlatformPlayer;
import :Platforms;

using namespace z0;

void PlatformsScene::onReady() {
    setProcessMode(PROCESS_MODE_ALWAYS);
    log("Loading scene");
    Loader::addSceneFromFile(this, "res/scenes/scene3.json");
    const auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
    log("VRAM usage", to_string(usage) + "%");

    // get the nodes declared in the JSON file
    game = getChild("game");
    if (game == nullptr) { die("game not found"); }
    player = static_pointer_cast<PlatformPlayer>(game->getChild("player"));
    if (player == nullptr) { die("player not found"); }
    level = static_pointer_cast<PlatformPlayer>(game->getChild("level"));
    if (level == nullptr) { die("level not found"); }

    // load and start the first level
    enterLevel(1);

    printTree();
}

void PlatformsScene::onProcess(const float alpha) {
    if (player->getPositionGlobal().y < 0) {
        player->releaseMouse();
        app().setPaused(true);
    }
}

void PlatformsScene::enterLevel(const int num_level) const {
    // remove the previous level
    level->removeAllChildren();
    // load the scene
    auto root_node = Loader::loadModelFromFile("res/models/level_" + to_string(num_level) + ".glb");
    // change the parent node of the first-level nodes of the scene
    // and attach them to the `level` node
    // while checking special nodes
    auto it = root_node->getChildren().begin();
    while (it != root_node->getChildren().end()) {
        auto child = *it;
        if (child->getName() == "SpawnPoint") {
            player->setPositionGlobal(child->getPositionGlobal());
            ++it;
        } else {
            it = root_node->removeChild(it);
            level->addChild(child);
        }
    }
    // crate the physics bodies
    createBodies(level);
}

// create the physics bodies using the node names
// rocks -> static bodies
void PlatformsScene::createBodies(const shared_ptr<Node> &node) const {
    auto it = node->getChildren().begin();
    while (it != node->getChildren().end()) {
        auto child = *it;
        if (child->getType() == NODE) {
            createBodies(child);
        } else if (child->getName().starts_with("rock.")) {
            auto body = make_shared<StaticBody>(make_shared<ConvexHullShape>(child), WORLD);
            // re-parent the node under the body
            auto parent = child->getParent();
            it          = parent->removeChild(it);
            body->addChild(child);
            parent->addChild(body);
            continue;
        }
        ++it;
    }
}
