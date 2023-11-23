#pragma once
#include "pch.h"
#include "entity.h"
#include "graphics/camera.h"
#include "physics/collision.h"
#include "physics/debugdraw.h"

#define TESTING


class Level
{
private:
    HashMap<String, sf::Texture> m_textures;
    Vec<Sprite> m_environment;
    Vec<Entity> m_entities;
    Box<b2World> m_world;
    ContactListener m_listener;
    Camera m_camera;
    b2Body* p_player;

#ifdef TESTING
    mutable DebugDraw m_debugdraw;
#endif

public:
    Level();

#ifdef TESTING
    void load_test_level();
#endif
    void load(const std::string& filename);

    void destroy_queued();

    void handle_event(const sf::Event& event);
    void update(const f32 dt);
    void render(sf::RenderTarget& target) const;
};

