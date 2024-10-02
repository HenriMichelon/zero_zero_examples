module;
#include "libraries.h"

export module Example:Platforms;

import :Layers;
import :PlatformPlayer;

export class PlatformsScene : public Node {
public:
    void onReady() override;

    void onProcess(const float alpha) override;

    ~PlatformsScene() override = default;

private:
    shared_ptr<PlatformPlayer> player;
    // the pausable node
    shared_ptr<Node>           game;
    // where to load the level
    shared_ptr<Node>           level;

    void enterLevel(const int num_level) const;

    // create the physics bodies using the node names
    // rocks -> static bodies
    void createBodies(const shared_ptr<Node> &node) const;
};
