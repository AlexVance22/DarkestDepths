#include "pch.h"
#include "player.h"
#include "entity.h"
#include "input.h"


void PlayerController::handle_event(Entity* self, Input::Action action) {
    auto rb = self->m_rigidbody;

    switch (action)
    {
    case Input::Action::Dash:
        if (!isdashing && iswalking) {
            isdashing = true;
            iswalking = false;
            dashtime = dashlength;
            dashdir = (i8)(direction.x / std::abs(direction.x));
        }
        break;

    case Input::Action::Jump:
        if (!isairborne) {
            const auto x = rb->GetLinearVelocity().x;
            rb->SetLinearVelocity(b2Vec2(x, -jumppower));
        }
        break;

    case Input::Action::BasicAttack: {
        const auto& data = std::get<PlayerData>(self->m_data);
        b2Body* closest = nullptr;
        f32 dist = 0.f;
        for (const auto body : data.hittargets) {
            const auto d = (rb->GetPosition() - body->GetPosition()).LengthSquared();
            if (!closest || d < dist) {
                closest = body;
                dist = d;
            }
        }
        if (closest) { printf("hit a guy\n"); }
        break; }
    default:
        break;
    }
}

void PlayerController::update(Entity* self, const f32 dt) {
    auto rb = self->m_rigidbody;

    // dashing
    if (isdashing) {
        direction.x = dashdir * dashpower;
        dashtime -= dt;
        isdashing = dashtime >= 0.f;

    // walking
    } else {
        // facing
        const i8 dir = Input::is_action_pressed(Input::Action::MoveRight)
                     - Input::is_action_pressed(Input::Action::MoveLeft);
        const auto newfacing = dir != 0 ? dir : facing;
        if (newfacing != facing) {
            auto hitter = (b2PolygonShape*)rb->GetFixtureList()->GetShape();
            hitter->m_vertices[0] = b2Vec2(newfacing * 0.4f - 0.3f, -0.1f - 0.3f);
            hitter->m_vertices[1] = b2Vec2(newfacing * 0.4f + 0.3f, -0.1f - 0.3f);
            hitter->m_vertices[2] = b2Vec2(newfacing * 0.4f + 0.3f, -0.1f + 0.3f);
            hitter->m_vertices[3] = b2Vec2(newfacing * 0.4f - 0.3f, -0.1f + 0.3f);
            facing = newfacing;
        }

        // moving
        direction.x = dir * walkspeed;
        iswalking = dir != 0;
    }

    const f32 y = rb->GetLinearVelocity().y;
    rb->SetLinearVelocity(b2Vec2(direction.x, y));
}

