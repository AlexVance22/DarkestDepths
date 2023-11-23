#include "pch.h"
#include "debugdraw.h"


DebugDraw::DebugDraw() { }

void DebugDraw::set_target(sf::RenderTarget& target) {
    p_target = &target;
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    sf::ConvexShape shape;
    shape.setPointCount(vertexCount);
    for (int32 i = 0; i < vertexCount; i++) {
        auto pos = sf::Vector2f(vertices[i].x * 50.f, vertices[i].y * 50.f);
        shape.setPoint(i, pos);
    }
    shape.setFillColor(sf::Color::Transparent);
    // shape.setOutlineColor(sf::Color(color.r, color.g, color.b));
    shape.setOutlineColor(sf::Color::Yellow);
    shape.setOutlineThickness(1);
    p_target->draw(shape);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    sf::ConvexShape shape;
    shape.setPointCount(vertexCount);
    for (int32 i = 0; i < vertexCount; i++) {
        auto pos = sf::Vector2f(vertices[i].x * 50.f, vertices[i].y * 50.f);
        shape.setPoint(i, pos);
    }
    // shape.setFillColor(sf::Color(color.r, color.g, color.b));
    shape.setFillColor(sf::Color::Magenta);
    p_target->draw(shape);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {}
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {}
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {}
void DebugDraw::DrawTransform(const b2Transform& xf) {}
void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {}

