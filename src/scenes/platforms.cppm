module;
#include "libraries.h"
import Z0;
using namespace z0;

export module Example:Platforms;

import :Layers;

export class PlatformsScene : public Node {
public:
    void onReady() override {
        setProcessMode(PROCESS_MODE_ALWAYS);
        log("Loading scene");
        Loader::addSceneFromFile(this, "res/scenes/scene2.json");
        const auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
        log("VRAM usage", to_string(usage) + "%");
        printTree();
    }

    ~PlatformsScene() override = default;
};