module;
#include "libraries.h"

module example.TopBar;

TopBar::TopBar(Object *obj, const std::function<void()> &_onQuit):
    ui::Window(ui::Rect{0, 800, 1000, 100}),
    onQuitHandler{obj},
    onQuit{_onQuit} {
}

void TopBar::onCreate() {
    getWidget().setDrawBackground(false);
    getWidget().setFont(make_shared<Font>(getWidget().getFont().getFontName(), 30));

    const auto rightPadding = make_shared<ui::Panel>();
    rightPadding->setDrawBackground(false);
    getWidget().add(rightPadding, ui::Widget::RIGHT, "5,5");

    textFPS = make_shared<ui::Text>("9999");
    getWidget().add(textFPS, ui::Widget::RIGHTCENTER);
    textFPS->setTextColor(vec4{1.0, 1.0, 0.2, 1.0});

    const auto buttonQuit = make_shared<ui::Button>();
    buttonQuit->connect(ui::Event::OnClick, [this]{ this->onQuit(); });
    getWidget().add(buttonQuit, ui::Widget::LEFTCENTER);
    const auto textQuit = make_shared<ui::Text>("Menu");
    buttonQuit->add(textQuit, ui::Widget::CENTER);
    buttonQuit->setSize(textQuit->getWidth() + 20, textQuit->getHeight() + 20);
    setHeight(buttonQuit->getHeight());
    setY(1000 - getHeight());

    const auto buttonPause = make_shared<ui::Button>();
    buttonPause->connect(ui::Event::OnClick, [this](void* p) { this->onPauseToggle(static_cast<ui::EventClick *>(p)); });
    getWidget().add(buttonPause, ui::Widget::LEFTCENTER);
    const auto textPause = make_shared<ui::Text>("Pause");
    buttonPause->add(textPause, ui::Widget::CENTER);
    buttonPause->setSize(textPause->getWidth() + 20, textPause->getHeight() + 20);

    hide();
}

void TopBar::updateFPS() {
    if (isVisible()) {
        if (Application::get().isPaused()) {
            textFPS->setText("Pause");
        } else {
            const auto newFPS = Application::get().getFPS();
            if (newFPS != fps) {
                fps = newFPS;
                textFPS->setText(to_string(fps));
            }
        }
    }
}

void TopBar::onPauseToggle(ui::EventClick *event) const {
    Application::get().setPaused(!Application::get().isPaused());
    log("Pause ", to_string(Application::get().isPaused()));
    event->consumed = true;
}
