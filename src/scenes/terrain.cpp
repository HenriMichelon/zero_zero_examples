#include "includes.h"
#include "layers.h"
#include "nodes/player.h"
#include "nodes/crate.h"
#include "nodes/terrain.h"
#include "scenes/terrain.h"

void TerrainScene::onReady() {
    setProcessMode(PROCESS_MODE_ALWAYS);
    log("Loading scene");
    Loader::addSceneFromFile(this, "res/scenes/scene1.json");
    auto player = getNode("game/player");
    log(to_string(player->getPosition()));
    log(to_string(player->getPositionGlobal()));
    auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
    log("VRAM usage", to_string(usage)+"%");
    printTree();
}
