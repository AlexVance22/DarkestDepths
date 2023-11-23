#pragma once
#include "pch.h"


enum class ColliderType {
    Ground            = 0,
    PlayerMain,
    PlayerHitBox,
    PlayerBasicRange,
    PlayerLungeRange,
    ItemInner,
    ItemOuter,
    GroundSensor,
    HitSensor,
    EnemyHitBox,
};


struct BoxCollider {
    ColliderType type;
    sf::Vector2f halfsize;
    sf::Vector2f offset;
    bool issensor = false;
    f32 density = 0.f;
    u16 isgroups = 0;
    u16 collideswith = 0;

    b2Fixture* add_to_body(b2Body* rb) {
        auto fixturedef = b2FixtureDef();
        fixturedef.density = density;
        auto shape = b2PolygonShape();
        const b2Vec2 vertices[] = {
            b2Vec2(offset.x - halfsize.x, offset.y - halfsize.y),
            b2Vec2(offset.x + halfsize.x, offset.y - halfsize.y),
            b2Vec2(offset.x + halfsize.x, offset.y + halfsize.y),
            b2Vec2(offset.x - halfsize.x, offset.y + halfsize.y),
        };
        shape.Set(vertices, 4);
        fixturedef.shape = &shape;
        fixturedef.isSensor = issensor;
        fixturedef.userData.pointer = (uintptr_t)type;
        fixturedef.filter.categoryBits = isgroups;
        fixturedef.filter.maskBits = collideswith;
        return rb->CreateFixture(&fixturedef);
    }
};


struct DynamicBody {
    sf::Vector2f position;
    bool rotates = false;

    b2Body* add_to_world(b2World* world) {
        auto bodydef = b2BodyDef();
        bodydef.type = b2_dynamicBody;
        bodydef.position.Set(position.x, position.y);
        bodydef.fixedRotation = !rotates;
        return world->CreateBody(&bodydef);
    }
};

struct KinematicBody {
    sf::Vector2f position;
    bool rotates = false;

    b2Body* add_to_world(b2World* world) {
        auto bodydef = b2BodyDef();
        bodydef.type = b2_kinematicBody;
        bodydef.position.Set(position.x, position.y);
        bodydef.fixedRotation = !rotates;
        return world->CreateBody(&bodydef);
    }
};

struct StaticBody {
    sf::Vector2f position;
    bool rotates = false;

    b2Body* add_to_world(b2World* world) {
        auto bodydef = b2BodyDef();
        bodydef.type = b2_staticBody;
        bodydef.position.Set(position.x, position.y);
        bodydef.fixedRotation = !rotates;
        return world->CreateBody(&bodydef);
    }
};

