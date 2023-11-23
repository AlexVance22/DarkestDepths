#pragma once
#include "pch.h"
#include "entity/player.h"


// Enemy power hierarchy
//
// Wisp < Shroud < Soul < Minion < Demon < Wraith < Vice < Prince
//

struct ItemMovement {
    b2Body* target = nullptr;
    f32 height = 0.5f;
};

struct SoulMovement {
    f32 value;
};

struct WraithMovement {
    f32 value;
};

struct BossMovement {
    f32 value;
};


Variant(Movement,
    PlayerController,
    ItemMovement,
    SoulMovement,
    WraithMovement,
    BossMovement
);

