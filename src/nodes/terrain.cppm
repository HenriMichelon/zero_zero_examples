module;
#include "libraries.h"
import Z0;
using namespace z0;

export module Example:Terrain;

import :Layers;

export class Terrain : public StaticBody {
public:
    Terrain(): StaticBody{Layers::WORLD, "terrain"} {
    }

    void onReady() override {
        auto terrainModel = findFirstChild<MeshInstance>();
        vector<SubShape> terrainCollisionShapes;
        terrainCollisionShapes.push_back(SubShape{make_shared<MeshShape>(terrainModel)});
        // virtual walls
        terrainCollisionShapes.push_back(
            SubShape{make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, -100.0}});
        terrainCollisionShapes.push_back(SubShape{
            make_shared<BoxShape>(vec3{100.0, 10.0, 1.0}), vec3{0.0, 5.0, 100.0}
        });
        terrainCollisionShapes.push_back(SubShape{
            make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{100.0, 5.0, 0.0}
        });
        terrainCollisionShapes.push_back(
            SubShape{make_shared<BoxShape>(vec3{1.0, 10.0, 100.0}), vec3{-100.0, 5.0, 0.0}});
        setShape(make_shared<StaticCompoundShape>(terrainCollisionShapes));
    }
};
