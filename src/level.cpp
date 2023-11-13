#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "level.h"


Level::Level()
{

}

void Level::load(const std::string& filename)
{
    const auto file = fs::read_to_string(filename).unwrap();
    rapidjson::Document doc;
    doc.Parse(file.c_str());

    for (const auto& tex : doc["textures"].GetArray())
    {
        sf::Texture t;
        if(!t.loadFromFile(tex["file"].GetString())) {
            panic("falied to load texture '%s'", tex["file"].GetString());
        }
        m_textures.insert(tex["name"].GetString(), t);
    }

    for (const auto& env : doc["world"].GetArray())
    {
        auto tf = env["transform"].GetObject();
        Sprite sprite;
        sprite.setPosition(sf::Vector2f(tf["loc"].GetArray()[0].GetFloat(), tf["loc"].GetArray()[1].GetFloat()));
        sprite.setRotation(tf["rot"].GetFloat());
        sprite.setScale(sf::Vector2f(tf["scale"].GetArray()[0].GetFloat(), tf["scale"].GetArray()[1].GetFloat()));

        auto sp = env["sprite"].GetObject();

        if (!sp["texture"].IsNull()) {
            sprite.set_texture(*m_textures.get(sp["texture"].GetString()).unwrap());
        }
        if (!sp["color"].IsNull()) {
            auto col = sp["color"].GetArray();
            sprite.set_color(sf::Color(col[0].GetUint(), col[1].GetUint(), col[2].GetUint()));
        }
        if (!sp["anim"].IsNull()) {

        }
        if (!sp["size"].IsNull()) {
            auto sz = sp["size"].GetArray();
            sprite.set_size(sf::Vector2f(sz[0].GetFloat(), sz[1].GetFloat()));
        }
        sprite.set_depth(sp["depth"].GetFloat());
        // "depth": 10

        m_environment.push(sprite);
    }
}

void Level::update(const f32 dt)
{

}

void Level::render(sf::RenderTarget& target) const
{
    auto sprites = Vec<const Sprite*>::with_capacity(m_entities.len());

    for (const auto& e : m_entities) {
        sprites.push(&e.get_sprite());
    }
    for (const auto& s : m_environment) {
        sprites.push(&s);
    }

    sprites.sort_by([](const Sprite* left, const Sprite* right){ return left->depth() > right->depth(); });

    for (auto s : sprites)
    {
        target.draw(*s);
    }
}

