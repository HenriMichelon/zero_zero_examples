#include <z0/macros.h>
#include "libraries.h"

import example;
import game;
import layers;
using namespace space_station;

const ApplicationConfig applicationConfig{
    .appName = "Example App",
    .appDir = ".",
    .layerCollisionTable = LayerCollisionTable{
        LAYERS_COUNT,
        {
           { PLAYER, { WORLD, BODIES, USABLE_PROP }},
           { BODIES, { WORLD, BODIES, PLAYER, USABLE_PROP }},
           { PLAYER_RAYCAST, { BODIES }},
           { TRIGGERS, { PLAYER }},
        { INTERACTIONS, { USABLE_PROP }},
        }
    },
    .windowMode = WindowMode::WINDOWED,
    .windowWidth = 1280,
    .windowHeight = 720,
    .windowMonitor = 0,
    .defaultFontName = "app://res/Signwood.ttf",
    .defaultFontSize = 25,
    .loggingMode = LOGGING_MODE_FILE | LOGGING_MODE_STDOUT,
    .logLevelMin = LogLevel::INFO,
    .msaa = MSAA::X4,
    .depthFormat = DepthFormat::B24,
    .useDepthPrepass = true,
    .gamma = 1.0f,
    .exposure = 1.f,
    .framesInFlight = 1,
    .pointLightShadowMapSize = 1024,
    .debug = false,
    .debugConfig = {
        .displayAtStartup = true,
        .updateDelay = 5,
        .drawWithDepthTest = true,
        .drawCoordinateSystem = true,
        .drawCoordinateSystemPosition = vec3{0.0f, 1.0f, 0.0f},
        .drawCoordinateSystemScale = 2.0f,
        .drawRayCast = true,
        .drawGetSupportingFace = false,
        .drawShape = true,
        .drawShapeWireframe = true,
        .drawShapeColor = DebugShapeColor::InstanceColor,
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

Z0_REGISTER_TYPE(TunnelDoor)
Z0_REGISTER_TYPE(SlideDoor)
Z0_REGISTER_TYPE(Console)

Z0_APP(applicationConfig, make_shared<ExampleMainScene>())
