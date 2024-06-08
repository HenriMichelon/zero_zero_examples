#include "z0/z0.h"
#ifndef USE_PCH
#include "z0/nodes/node.h"
//#include "z0/nodes/raycast.h"
#endif
#include "z0/raycast.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuadTree.h>

namespace z0 {

    RayCast::RayCast(const string& name): Node(name) {
        //broadPhase = make_unique<JPH::BroadPhaseQuadTree>();
	    //broadPhase->Init(mBodyManager, mBroadPhaseLayerInterface);
    }

    void RayCast::_physicsUpdate() {
        auto position = getPositionGlobal();
        JPH::RayCast ray{
            JPH::Vec3(position.x, position.y, position.z),
            JPH::Vec3(target.x, target.y, target.z)
        };
        JPH::AllHitCollisionCollector<JPH::RayCastBodyCollector> collector;
	    broadPhase->CastRay(ray, collector);
	    int num_hits = (int)collector.mHits.size();
	    auto *results = collector.mHits.data();
        log("raycast hit ", to_string(collector.mHits.size()));
    }

}
