#pragma once
#include "pch.h"


struct PlayerTracker {
    b2Body* target;
};

struct PathTracker {
};

struct FixedPosition {
    sf::Vector2f pos;
    f32 smoothspeed;
};


Enum(CameraMotion,
    PlayerTracker,
    PathTracker,
    FixedPosition
);


class Camera
{
private:
    sf::View m_view;
    CameraMotion m_motion;

public:
    Camera() = default;
    Camera(sf::FloatRect rect);

    void reset(sf::FloatRect rect);
    void set_viewport(sf::FloatRect viewport);
    void move(sf::Vector2f motion);

    void set_size(sf::Vector2f size);
    void set_center(sf::Vector2f center);
    void set_rotation(f32 rotation);

    sf::Vector2f get_size() const;
    sf::Vector2f get_center() const;
    f32 get_rotation() const;

    void set_mode(const CameraMotion::Value_t& mode);

    void update(const float dt);

    sf::View get_view() const;
};

