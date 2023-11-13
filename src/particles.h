#include <SFML/Graphics.hpp>
#include "core/mod.h"


class ParticleSystem : public sf::Drawable
{
private:
    Vec<sf::Vertex> m_vertices;

public:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

