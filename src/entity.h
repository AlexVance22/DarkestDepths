#pragma once
#include "pch.h"
#include "graphics/sprite.h"
#include "physics/movement.h"
#include "entity/item.h"


enum class EntityKind {
    World  = 1,
    Player = 1 << 1,
    Item   = 1 << 2,
    Soul   = 1 << 3,
    Wraith = 1 << 4,
    Boss   = 1 << 5,
};

Variant(EntityData,
    PlayerData,
    ItemData
);


class Entity
{
private:
    EntityKind m_kind;
    b2Body* m_rigidbody;
    Sprite m_sprite;
    Movement m_movement;
    EntityData m_data;
    bool m_destroy = false;

    Entity(EntityKind kind, b2Body* rigidbody): m_kind(kind), m_rigidbody(rigidbody) {}

public:
    static Entity new_player(b2Body* rb);
    static Entity new_item(b2Body* rb);
    static Entity new_soul(b2Body* rb);

    Sprite& get_sprite() { return m_sprite; }
    const Sprite& get_sprite() const { return m_sprite; }
    Movement& get_controller() { return m_movement; }
    const Movement& get_controller() const { return m_movement; }
    b2Body* get_body();
    const b2Body* get_body() const;

    void collide(b2Fixture* self, b2Fixture* other);
    void decollide(b2Fixture* self, b2Fixture* other);

    bool destroy() const { return m_destroy; }

    void handle_event(const sf::Event& event);
    void update(const f32 dt);

    friend struct PlayerController;
};

