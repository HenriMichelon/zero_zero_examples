module;
#include "libraries.h"

export module game.Interactions;

import layers;

export namespace space_station {

    class UsableProp : public StaticBody {
    public:
        virtual void use() = 0;

        void onReady() override;
        void setProperty(const string &property, const string &value) override;
    };

    class Interactions : public RayCast {
    public:
        static inline Signal::signal on_display_info = "on_interactions_display_info";
        static inline Signal::signal on_hide_info = "on_interactions_hide_info";

        explicit Interactions();
        void     onReady() override;
        void     onProcess(float alpha) override;
        bool     onInput(InputEvent &inputEvent) override;

    private:
        shared_ptr<Node> targetNode;
    };

    class Console : public UsableProp {
    public:
        void onReady() override;
        void use() override;
    };

}