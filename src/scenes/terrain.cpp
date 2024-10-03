module;
#include "libraries.h"

module Example;

import :TerrainScene;

void TerrainScene::onReady() {
    setProcessMode(PROCESS_MODE_ALWAYS);
    log("Loading scene");
    Loader::addSceneFromFile(this, "res/scenes/scene1.json");
}
