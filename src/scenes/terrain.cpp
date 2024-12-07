module;
#include "libraries.h"

module Example;

import :TerrainScene;

void TerrainScene::onReady() {
    setProcessMode(ProcessMode::ALWAYS);
    log("Loading scene");
    // addChild(Loader::load("app://res/scenes/scene1.json"));
}
