#include <z0/macros.h>
#include "libraries.h"

import Example;

const ApplicationConfig applicationConfig{
    .appName = "Example App",
    .appDir = ".",
    .layerCollisionTable = LayerCollisionTable{
        LAYERS_COUNT,
        {
                { PLAYER, { WORLD, BODIES }},
                { BODIES, { WORLD, BODIES, PLAYER }},
                { PLAYER_RAYCAST, { BODIES }},
        }
    },
    .windowMode = WindowMode::WINDOWED,
    .windowWidth = 1280,
    .windowHeight = 720,
    .windowMonitor = 0,
    .defaultFontName = "app://res/Signwood.ttf",
    .defaultFontSize = 25,
    .loggingMode = LOGGING_MODE_FILE | LOGGING_MODE_STDOUT,
    .useDepthPrepass = true,
    .framesInFlight = 3,
    .debug = false,
    .debugConfig = {
        .updateDelay = 10,
        .drawWithDepthTest = true,
        .drawCoordinateSystem = false,
        .drawCoordinateSystemPosition = vec3{0.0f, 1.0f, 0.0f},
        .drawCoordinateSystemScale = 2.0f,
        .drawRayCast = true,
        .drawGetSupportingFace = false,
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
