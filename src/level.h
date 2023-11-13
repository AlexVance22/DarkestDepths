#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "core/mod.h"
#include "entity.h"


class Level
{
private:
    HashMap<String, sf::Texture> m_textures;

    Vec<Sprite> m_environment;
    Vec<Entity> m_entities;
    Box<b2World> m_world;

public:
    Level();

    void load(const std::string& filename);
    void update(const f32 dt);
    void render(sf::RenderTarget& target) const;
};

