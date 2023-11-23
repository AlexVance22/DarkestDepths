#include "pch.h"
#include "camera.h"


Camera::Camera(sf::FloatRect rect) {
    m_view.reset(rect);
}

void Camera::reset(sf::FloatRect rect) {
    m_view.reset(rect);
}
void Camera::set_viewport(sf::FloatRect viewport) {
    m_view.setViewport(viewport);
}
void Camera::move(sf::Vector2f motion) {
    m_view.move(motion);
}

void Camera::set_size(sf::Vector2f size) {
    m_view.setSize(size);
}
void Camera::set_center(sf::Vector2f center) {
    m_view.setCenter(center);
}
void Camera::set_rotation(f32 rotation) {
    m_view.setRotation(rotation);
}

sf::Vector2f Camera::get_size() const {
    return m_view.getSize();
}
sf::Vector2f Camera::get_center() const {
    return m_view.getCenter();
}
f32 Camera::get_rotation() const {
    return m_view.getRotation();
}

void Camera::set_mode(const CameraMotion::Value_t& mode) {
    switch (mode.index()) {
        case 0:
            m_motion.kind = CameraMotion::PlayerTracker;
            break;
        case 1:
            m_motion.kind = CameraMotion::PathTracker;
            break;
        case 2:
            m_motion.kind = CameraMotion::FixedPosition;
            break;
    }
    m_motion.data = mode;
}

template<typename T>
T sign(T val) {
    return val / std::abs(val);
}

void Camera::update(const float dt) {
    switch (m_motion.kind) {
        case CameraMotion::PlayerTracker: {
            auto rb = std::get<PlayerTracker>(m_motion.data).target;
            auto pos = rb->GetPosition();
            auto dir = rb->GetLinearVelocity();
            auto vec = sf::Vector2f(pos.x, pos.y - 1.f) * 50.f - get_center();
            if (std::abs(dir.x) > 0.001f) {
                vec.x += sign(dir.x) * 50.f;
                move(vec * 0.1f);
            } else {
                move(vec * 0.05f);
            }
            break; }
        case CameraMotion::PathTracker:
            break;
        case CameraMotion::FixedPosition: {
            auto data = std::get<FixedPosition>(m_motion.data);
            auto vec = data.pos - get_center();
            move(vec * data.smoothspeed);
            break; }

    }
}

sf::View Camera::get_view() const {
    return m_view;
}

