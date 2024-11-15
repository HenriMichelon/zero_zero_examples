module;
#include "libraries.h"

module Example;

import :TerrainScene;

void TerrainScene::onReady() {
    setProcessMode(ProcessMode::ALWAYS);
    log("Loading scene");
    Loader::addScene(this, "app://res/scenes/scene1.json");
}
