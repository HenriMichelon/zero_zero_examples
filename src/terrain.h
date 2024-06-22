#pragma once

class TerrainScene: public Node {
public:
    void onReady() override;

private:
    shared_ptr<Player> player;
};
