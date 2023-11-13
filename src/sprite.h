#include <SFML/Graphics.hpp>
#include "core/mod.h"


class Sprite : public sf::Drawable, public sf::Transformable
{
private:
    sf::VertexArray m_vertices;
    const sf::Texture* p_texture;
    f32 m_depth;

    bool m_animated = false;
    bool m_playing = false;

    f32 m_timetotal = 0.f;
    u32 m_frametotal = 0;
    u32 m_animtotal = 0;

    f32 m_time = 0.f;
    u32 m_frame = 0;
    u32 m_anim = 0;

    sf::Vector2f m_framesize;

public:
    Sprite();

    void set_depth(f32 depth) { m_depth = depth; }
    f32 depth() const { return m_depth; }

    void set_color(sf::Color color);
    void set_size(sf::Vector2f size);
    void set_texture(const sf::Texture& texture);
    void set_texture_box(sf::Vector2f pos, sf::Vector2f size);
    void set_animation(u32 frames, u32 anims, f32 framerate);

    void set_frame(u32 index);
    void set_anim(u32 index);
    void set_framerate(f32 framerate);

    void pause_anim();
    void play_anim();

    bool update_anim(const f32 dt);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

