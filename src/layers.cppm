export module layers;

// values used in the layers & masks for the collisions detection
export enum Layers {
    NONE        = 0,
    WORLD       = 1,
    BODIES      = 2,
    PLAYER      = 3,
    TRIGGERS    = 4,
    USABLE_PROP = 5,
    INTERACTIONS= 6,
    PLAYER_RAYCAST     = 7,
    LAYERS_COUNT= PLAYER_RAYCAST+1,
};

