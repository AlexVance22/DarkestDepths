#pragma once
#include "pch.h"
#include "input.h"

class Entity;

struct PlayerData {
    HashSet<b2Body*> hittargets;
};

struct PlayerController {
    f32 walkspeed;
    f32 jumppower;
    f32 dashpower;
    f32 dashlength;

    bool isairborne = false;
    bool isdashing = false;
    bool iswalking = false;
    f32 dashtime = 0.f;
    i8 facing = 0;
    i8 dashdir = 0;

    sf::Vector2f direction;

    void handle_event(Entity* self, Input::Action action);
    void update(Entity* self, const f32 dt);
};

