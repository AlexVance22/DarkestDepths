#include "core/mod.h"
#include "sprite.h"


struct SoulMovement {
    f32 value;
};

struct WraithMovement {
    f32 value;
};

struct PlayerMovement {
    f32 value;
};

struct BossMovement {
};

struct ItemMovement {
};


struct Movement {
    enum {
        Soul,
        Wraith,
        Boss,
        Item,
        Player,
    } kind;

    union {
        SoulMovement soul;
        WraithMovement wraith;
        BossMovement boss;
        ItemMovement item;
        PlayerMovement player;
    } data;

    static Movement soul(SoulMovement m) { Movement res; res.kind = Movement::Soul; res.data.soul = m; return res; }
    static Movement wraith(WraithMovement m) { Movement res; res.kind = Movement::Wraith; res.data.wraith = m; return res; }
    static Movement boss(BossMovement m) { Movement res; res.kind = Movement::Boss; res.data.boss = m; return res; }
    static Movement item(ItemMovement m) { Movement res; res.kind = Movement::Item; res.data.item = m; return res; }
    static Movement player(PlayerMovement m) { Movement res; res.kind = Movement::Player; res.data.player = m; return res; }
};


class Entity
{
private:
    Sprite sprite;
    Movement movement;

public:
    Entity();

    const Sprite& get_sprite() const { return sprite; }

    void update(const f32 dt);
};

