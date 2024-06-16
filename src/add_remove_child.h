using namespace z0;

class AddRemoveChildMainScene: public Node, public GEventHandler {
public:
    AddRemoveChildMainScene(): Node{"Main Scene"} {};
    void onReady() override;
    bool onInput(InputEvent& event) override;
    void onPhysicsProcess(float delta) override;
    void onEnterScene() override;
    void onExitScene() override;
private:
    shared_ptr<Node> crateModel;
    shared_ptr<Node> sphereModel;
    list<shared_ptr<Node>> rotatingNodes;
    shared_ptr<Camera> camera1;
    shared_ptr<Camera> camera2;
    shared_ptr<Camera> currentCamera;
    shared_ptr<GWindow> menu;

    void onMenuAdd(GWidget*w=nullptr, GEvent*e=nullptr);
    void onMenuRemove(GWidget*w=nullptr, GEvent*e=nullptr);
    void onMenuCamera(GWidget*w=nullptr, GEvent*e=nullptr);
};
