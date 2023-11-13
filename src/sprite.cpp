#include <SFML/Graphics.hpp>
#include "core/mod.h"

#include <sprite.h>


Sprite::Sprite()
    : m_vertices(sf::Quads, 4)
{}

void Sprite::set_size(sf::Vector2f size)
{
    m_vertices[0].position = sf::Vector2f(0.0, 0.0);
    m_vertices[1].position = sf::Vector2f(size.x, 0.0);
    m_vertices[2].position = size;
    m_vertices[3].position = sf::Vector2f(0.f, size.y);
}

void Sprite::set_color(sf::Color color)
{
    m_vertices[0].color = color;
    m_vertices[1].color = color;
    m_vertices[2].color = color;
    m_vertices[3].color = color;
}

void Sprite::set_texture(const sf::Texture& texture)
{
    p_texture = &texture;
    set_texture_box(sf::Vector2f(0.f, 0.f), (sf::Vector2f)texture.getSize());
}

void Sprite::set_texture_box(sf::Vector2f pos, sf::Vector2f size)
{
    m_vertices[0].position = sf::Vector2f(0.0, 0.0);
    m_vertices[1].position = sf::Vector2f(size.x, 0.0);
    m_vertices[2].position = size;
    m_vertices[3].position = sf::Vector2f(0.f, size.y);
    m_vertices[0].texCoords = pos;
    m_vertices[1].texCoords = pos + sf::Vector2f(size.x, 0.f);
    m_vertices[2].texCoords = pos + size;
    m_vertices[3].texCoords = pos + sf::Vector2f(0.f, size.y);
}

void Sprite::set_animation(u32 frames, u32 anims, f32 framerate)
{
    m_frametotal = frames;
    m_animtotal = anims;
    m_animated = true;
    m_timetotal = 1.f / framerate;
    sf::Vector2u size = p_texture->getSize();
    m_framesize = sf::Vector2f((f32)(size.x / frames), (f32)(size.y / anims));
}

void Sprite::set_frame(u32 index)
{
    m_frame = index;
    set_texture_box(sf::Vector2f((f32)m_frame * m_framesize.x, (f32)m_anim * m_framesize.y), m_framesize);
}
void Sprite::set_anim(u32 index)
{
    m_anim = index;
    set_texture_box(sf::Vector2f((f32)m_frame * m_framesize.x, (f32)m_anim * m_framesize.y), m_framesize);
}
void Sprite::set_framerate(f32 framerate)
{
    m_timetotal = 1.f / framerate;
}

void Sprite::pause_anim()
{
    m_playing = true;
}
void Sprite::play_anim()
{
    m_playing = false;
}

bool Sprite::update_anim(const f32 dt)
{
    if (m_animated && m_playing)
    {
        m_time += dt;
        if (m_time > m_timetotal)
        {
            m_time -= m_timetotal;
            m_frame++;
            if (m_frame == m_frametotal)
            {
                m_frame = 0;
                set_frame(m_frame);
            }
            return true;
        }
    }

    return false;
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = p_texture;
    states.transform *= getTransform();
    target.draw(m_vertices);
}

