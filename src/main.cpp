#include <z0/macros.h>
#include "libraries.h"

import Example;

const ApplicationConfig applicationConfig{
    .appName = "Example App",
    .appDir = ".",
    .windowMode = WindowMode::WINDOWED,
    .windowWidth = 1280,
    .windowHeight = 768,
    .windowMonitor = 0,
    .defaultFontName = "app://res/Signwood.ttf",
    .defaultFontSize = 25,
    .loggingMode = LOGGING_MODE_FILE | LOGGING_MODE_WINDOW | LOGGING_MODE_STDOUT,
    .useDepthPrepass = true,
    .debug = true,
    .debugConfig = {
        .updateDelay = 10,
        .drawWithDepthTest = true,
        .drawCoordinateSystem = true,
        .drawCoordinateSystemPosition = vec3{0.0f, 2.0f, 0.0f},
        .drawCoordinateSystemScale = 5.0f,
        .drawShape = true,
        .drawShapeColor = DebugShapeColor::ShapeTypeColor,
        .drawBoundingBox = false,
        .drawVelocity = false,
        .drawMassAndInertia = false,
        .drawSleepStats = false,
        .drawCenterOfMassTransform = false
    }
};

// Register the examples node classes for the JSON loading
Z0_REGISTER_TYPE(Crate)
Z0_REGISTER_TYPE(Player)
Z0_REGISTER_TYPE(PlatformPlayer)
Z0_REGISTER_TYPE(Terrain)

Z0_APP(applicationConfig, make_shared<ExampleMainScene>())
