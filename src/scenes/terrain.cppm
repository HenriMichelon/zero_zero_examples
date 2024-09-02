module;
#include <z0/z0.h>

export module Example:TerrainScene;

export class TerrainScene: public Node {
public:
    void onReady() override;
    virtual ~TerrainScene() {};
};

void TerrainScene::onReady() {
    setProcessMode(PROCESS_MODE_ALWAYS);
    log("Loading scene");
    Loader::addSceneFromFile(this, "res/scenes/scene1.json");
    auto usage = (app().getVideoMemoryUsage() / static_cast<float>(app().getDedicatedVideoMemory())) * 100.0f;
    log("VRAM usage", to_string(usage)+"%");
    printTree();
}
