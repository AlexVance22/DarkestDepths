#include "pch.h"
#include "entity.h"
#include "input.h"
#include "physics/utils.h"


b2Vec2 sf_to_b2(sf::Vector2f vec) {
    return { vec.x, vec.y };
}
sf::Vector2f b2_to_sf(b2Vec2 vec) {
    return { vec.x, vec.y };
}


Entity Entity::new_player(b2Body* rb)
{
    BoxCollider{
        .type = ColliderType::PlayerMain,
        .halfsize = { 0.2f, 0.5f },
        .density = 40.f,
        .isgroups = (u16)EntityKind::Player,
        .collideswith = (u16)EntityKind::World | (u16)EntityKind::Item,
    }
    .add_to_body(rb);

    BoxCollider{
        .type = ColliderType::PlayerHitBox,
        .halfsize = { 0.18f, 0.45f },
        .issensor = true,
        .density = 0.f,
        .isgroups = (u16)EntityKind::Player,
        .collideswith = (u16)EntityKind::Soul,
    }
    .add_to_body(rb);

    BoxCollider{
        .type = ColliderType::GroundSensor,
        .halfsize = { 0.3f, 0.1f },
        .offset = { 0.f, 0.6f },
        .issensor = true,
        .density = 0.f,
        .isgroups = (u16)EntityKind::Player,
        .collideswith = (u16)EntityKind::World,
    }
    .add_to_body(rb);

    BoxCollider{
        .type = ColliderType::PlayerBasicRange,
        .halfsize = { 0.3f, 0.3f },
        .offset = { 0.4f, -0.1f },
        .issensor = true,
        .density = 0.f,
        .isgroups = (u16)EntityKind::Player,
        .collideswith = (u16)EntityKind::World | (u16)EntityKind::Item | (u16)EntityKind::Soul,
    }
    .add_to_body(rb);

    const auto pos = rb->GetPosition();

    auto e = Entity(EntityKind::Player, rb);
    e.m_movement = PlayerController{
        .walkspeed  = 3.f,
        .jumppower  = 6.f,
        .dashpower  = 15.f,
        .dashlength = 0.15f,
    };
    e.m_sprite.setOrigin(sf::Vector2f(0.2f * 50.f, 0.5f * 50.f));
    e.m_sprite.set_size(sf::Vector2f(0.4f * 50.f, 1.f * 50.f));
    e.m_sprite.setPosition(pos.x * 50.f, pos.y * 50.f);
    e.m_sprite.set_color(sf::Color::White);
    e.m_sprite.set_depth(0.f);
    return e;
}

Entity Entity::new_item(b2Body* rb)
{
    BoxCollider{
        .type = ColliderType::ItemOuter,
        .halfsize = { 0.8f, 0.8f },
        .issensor = true,
        .density = 0.f,
        .isgroups = (u16)EntityKind::Item,
        .collideswith = (u16)EntityKind::Player,
    }
    .add_to_body(rb);

    BoxCollider{
        .type = ColliderType::ItemInner,
        .halfsize = { 0.3f, 0.3f },
        .issensor = true,
        .density = 10.f,
        .isgroups = (u16)EntityKind::Item,
        .collideswith = (u16)EntityKind::Player,
    }
    .add_to_body(rb);

    const auto pos = rb->GetPosition();

    auto e = Entity(EntityKind::Item, rb);
    e.m_movement = ItemMovement{};
    e.m_sprite.setOrigin(sf::Vector2f(0.15f * 50.f, 0.15f * 50.f));
    e.m_sprite.set_size(sf::Vector2f(0.3f * 50.f, 0.3f * 50.f));
    e.m_sprite.setPosition(pos.x * 50.f, pos.y * 50.f);
    e.m_sprite.set_color(sf::Color::Magenta);
    e.m_sprite.set_depth(0.f);
    return e;
}

Entity Entity::new_soul(b2Body* rb)
{
    BoxCollider{
        .type = ColliderType::EnemyHitBox,
        .halfsize = { 0.2f, 0.5f },
        .density = 40.f,
        .isgroups = (u16)EntityKind::Soul,
        .collideswith = (u16)EntityKind::World | (u16)EntityKind::Player,
    }
    .add_to_body(rb);

    const auto pos = rb->GetPosition();

    auto e = Entity(EntityKind::Soul, rb);
    e.m_movement = SoulMovement{};
    e.m_sprite.setOrigin(sf::Vector2f(0.2f * 50.f, 0.5f * 50.f));
    e.m_sprite.set_size(sf::Vector2f(0.4f * 50.f, 1.f * 50.f));
    e.m_sprite.setPosition(pos.x * 50.f, pos.y * 50.f);
    e.m_sprite.set_color(sf::Color::Red);
    e.m_sprite.set_depth(0.f);
    return e;
}


b2Body* Entity::get_body() {
    return m_rigidbody;
}
const b2Body* Entity::get_body() const {
    return m_rigidbody;
}


void Entity::collide(b2Fixture* self, b2Fixture* other) {
    const auto self_type = (ColliderType)self->GetUserData().pointer;
    const auto other_type = (ColliderType)other->GetUserData().pointer;

    switch (m_kind)
    {
    case EntityKind::Player:
        switch (other_type)
        {
        case ColliderType::Ground:      // mark as grounded
            if (self_type == ColliderType::GroundSensor) {
                std::get<PlayerController>(m_movement).isairborne = false;
            }
            break;
        case ColliderType::ItemInner:   // collect items
            break;
        case ColliderType::EnemyHitBox: // add to hit targets
            std::get<PlayerData>(m_data).hittargets.insert(other->GetBody());
            break;
        default:
            break;
        }
        break;

    case EntityKind::Item:
        if (other_type == ColliderType::PlayerMain) {
            auto& data = std::get<ItemMovement>(m_movement);
            if (self_type == ColliderType::ItemOuter) {
                data.target = other->GetBody();
            } else if (self_type == ColliderType::ItemInner) {
                m_destroy = true;
            }
        }
        break;
    default:
        break;
    }
}

void Entity::decollide(b2Fixture* self, b2Fixture* other) {
    const auto self_type = (ColliderType)self->GetUserData().pointer;
    const auto other_type = (ColliderType)other->GetUserData().pointer;

    switch (m_kind)
    {
    case EntityKind::Player:
        switch (other_type)
        {
        case ColliderType::Ground:      // mark as grounded
            if (self_type == ColliderType::GroundSensor) {
                std::get<PlayerController>(m_movement).isairborne = true;
            }
            break;
        case ColliderType::EnemyHitBox: // add to hit targets
            std::get<PlayerData>(m_data).hittargets.remove(other->GetBody());
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}


void Entity::handle_event(const sf::Event& event)
{
    if (const auto action = Input::from_event(event).if_let()) {
        switch (m_kind)
        {
        case EntityKind::Player:
            std::get<PlayerController>(m_movement).handle_event(this, *action);
            break;
        default:
            break;
        }
    }
}

void Entity::update(const f32 dt)
{
    const auto pos = m_rigidbody->GetPosition();
    m_sprite.setPosition(pos.x * 50.f, pos.y * 50.f);

    switch (m_kind)
    {
    case EntityKind::Player:
        std::get<PlayerController>(m_movement).update(this, dt);
        break;

    case EntityKind::Item: {
        auto& data = std::get<ItemMovement>(m_movement);

        // pickup
        if (data.target) {
            const auto vec = data.target->GetPosition() - pos;
            m_rigidbody->SetLinearVelocity(b2Vec2(vec.x * 5.f, vec.y * 5.f));

        // bobbing
        } else {
            data.height += dt;
            if (data.height > 1.f) {
                data.height -= 2.f;
            } else if (data.height > 0.f) {
                const auto og = m_sprite.getOrigin();
                m_sprite.setOrigin(sf::Vector2f(og.x, og.y + 0.2f * dt * 60.f));
            } else {
                const auto og = m_sprite.getOrigin();
                m_sprite.setOrigin(sf::Vector2f(og.x, og.y - 0.2f * dt * 60.f));
            }
        }
        break; }

    case EntityKind::Soul:
        break;

    case EntityKind::Wraith:
        break;

    case EntityKind::Boss:
        break;

    default:
        break;
    }

    m_sprite.update_anim(dt);
}

