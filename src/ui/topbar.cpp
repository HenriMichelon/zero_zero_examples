module;
#include "libraries.h"

module Example;

import :TopBar;

TopBar::TopBar(Object *obj, const Signal::Handler _onQuit):
    GWindow(Rect{0, 800, 1000, 100}),
    onQuitHandler{obj},
    onQuit{_onQuit} {
}

void TopBar::onCreate() {
    getWidget().setDrawBackground(false);
    getWidget().setFont(make_shared<Font>(getWidget().getFont()->getFontName(), 30));

    const auto rightPadding = make_shared<GPanel>();
    rightPadding->setDrawBackground(false);
    getWidget().add(rightPadding, GWidget::RIGHT, "5,5");

    textFPS = make_shared<GText>("9999");
    getWidget().add(textFPS, GWidget::RIGHTCENTER);
    textFPS->setTextColor(Color{1.0, 1.0, 0.2});

    const auto buttonQuit = make_shared<GButton>();
    buttonQuit->connect(GEvent::OnClick, onQuitHandler, onQuit);
    getWidget().add(buttonQuit, GWidget::LEFTCENTER);
    const auto textQuit = make_shared<GText>("Menu");
    buttonQuit->add(textQuit, GWidget::CENTER);
    buttonQuit->setSize(textQuit->getWidth() + 20, textQuit->getHeight() + 20);
    setHeight(buttonQuit->getHeight());
    setY(1000 - getHeight());

    const auto buttonPause = make_shared<GButton>();
    buttonPause->connect(GEvent::OnClick, this, Signal::Handler(&TopBar::onPauseToggle));
    getWidget().add(buttonPause, GWidget::LEFTCENTER);
    const auto textPause = make_shared<GText>("Pause");
    buttonPause->add(textPause, GWidget::CENTER);
    buttonPause->setSize(textPause->getWidth() + 20, textPause->getHeight() + 20);

    // hide();
}

void TopBar::updateFPS() {
    if (isVisible()) {
        if (app().isPaused()) {
            textFPS->setText("Pause");
        } else {
            const auto newFPS = app().getFPS();
            if (newFPS != fps) {
                fps = newFPS;
                textFPS->setText(to_string(fps));
            }
        }
    }
}

void TopBar::onPauseToggle(GEventClick *event) const {
    Application::get().setPaused(!app().isPaused());
    log("Pause ", to_string(Application::get().isPaused()));
    event->consumed = true;
}
