module;
#include "libraries.h"

module game.Interactions;

namespace space_station {

    void UsableProp::onReady() {
        setCollisionLayer(USABLE_PROP);
    }

    void UsableProp::setProperty(const string &property, const string &value) {
        StaticBody::setProperty(property, value);
        if (property == "label") {
            setName(value);
        }
    }

    Interactions::Interactions(const shared_ptr<Node> &camera): camera{camera} {
    }

    void Interactions::onReady() {
        setCollisionLayer(INTERACTIONS);
        setTarget({0.0, 0.0, -1.0f});
    }

    bool Interactions::onInput(InputEvent &inputEvent) {
        if (targetNode && inputEvent.getType() == InputEventType::KEY) {
            const auto &event = dynamic_cast<InputEventKey &>(inputEvent);
            if ((event.getKey() == KEY_E) && event.isPressed()) {
                if (auto* usable = dynamic_cast<UsableProp*>(targetNode)) {
                    usable->use();
                    return true;
                }
            }
        }
        return false;
    }

    void Interactions::onProcess(float alpha) {
        const auto& object = getCollider();
        if (object) {
            if (object != targetNode) {
                // log("interaction detected", object->getName());
                targetNode = object;
                emit(on_display_info, targetNode);
            }
        } else if (targetNode) {
            targetNode = nullptr;
            emit(on_hide_info);
        }

    }

    void Console::onReady() {
        UsableProp::onReady();
        setName("Console");
    }

    void Console::use() {
        const auto& scene = getParent();
        for (const auto& light : scene->findAllChildren<OmniLight>()) {
            light->setVisible(!light->isVisible());
        }
        for (const auto& light : scene->findAllChildrenByGroup("light")) {
            light->setVisible(!light->isVisible());
        }
    }


}