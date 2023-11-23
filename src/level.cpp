#include "pch.h"
#include "level.h"
#include "physics/utils.h"
#include <memory>

#define tup(...) std::tuple<__VA_ARGS__>


Level::Level(): m_camera(sf::FloatRect(0, 0, 480, 270)) {
    m_entities.reserve(256);
    m_world = std::make_unique<b2World>(b2Vec2(0.0, 15.0));
    m_world->SetContactListener(&m_listener);
    m_world->SetDebugDraw(&m_debugdraw);
    m_debugdraw.SetFlags(b2Draw::e_shapeBit);
}


b2Body* static_box(b2World* world, sf::FloatRect box) {
    /*
    auto rb = StaticBody{ .position = box.getPosition() }.add_to_world(world);
    BoxCollider{
        .type = ColliderType::Ground,
        .halfsize = box.getSize() * 0.5f,
    }.add_to_body(rb);
    return rb;

    BoxCollider{
        .type = ColliderType::Ground,
        .halfsize = box.getSize() * 0.5f,
    }.add_to_body(StaticBody{
        .position = box.getPosition()
    }.add_to_world(world));

    StaticBody{ .position = box.getPosition() }
        .create(world)
        .add_collider(BoxCollider{
            .type = ColliderType::Ground,
            .halfsize = box.getSize() * 0.5f,
        });
    */

    auto bodydef = b2BodyDef();
    bodydef.type = b2_staticBody;
    bodydef.position.Set((box.left + box.width / 2.f) / 50.f, (box.top + box.height / 2.f) / 50.f);
    auto rb = world->CreateBody(&bodydef);
    auto fixturedef = b2FixtureDef();
    fixturedef.density = 0.f;
    auto shape = b2PolygonShape();
    shape.SetAsBox(box.width / 2.f / 50.f, box.height / 2.f / 50.f);
    fixturedef.shape = &shape;
    fixturedef.userData.pointer = (uintptr_t)ColliderType::Ground;
    rb->CreateFixture(&fixturedef);
    return rb;
}


b2Body* static_body(b2World* world, b2Vec2 pos, b2Shape* shape) {
    auto bodydef = b2BodyDef();
    bodydef.type = b2_staticBody;
    bodydef.position = pos;
    auto rb = world->CreateBody(&bodydef);
    auto fixturedef = b2FixtureDef();
    fixturedef.density = 0.f;
    fixturedef.shape = shape;
    fixturedef.userData.pointer = (uintptr_t)ColliderType::Ground;
    rb->CreateFixture(&fixturedef);
    return rb;
}

void Level::load_test_level()
{
    for (const auto& [name, file] : vec(tup(const char*, const char*), { "background", "res/textures/background.png" }, { "empty", "res/textures/empty.png" })) {
        sf::Texture tex;
        if(!tex.loadFromFile(file)) {
            panic("falied to load texture '%s'", file);
        }
        m_textures.insert(name, tex);
    }

    auto rb1 = DynamicBody{ .position = { 0.2f, 3.5f } }.add_to_world(m_world.get());
    m_entities.push(Entity::new_player(rb1));
    rb1->GetUserData().pointer = (uintptr_t)m_entities.last_mut().unwrap();

    m_camera.set_mode(PlayerTracker{ rb1 });
    p_player = rb1;

    auto rb2 = KinematicBody{ .position = { 220.f / 50.f, 160.f / 50.f } }.add_to_world(m_world.get());
    m_entities.push(Entity::new_item(rb2));
    rb2->GetUserData().pointer = (uintptr_t)m_entities.last_mut().unwrap();

    auto rb3 = DynamicBody{ .position = { 1.f, 3.5f } }.add_to_world(m_world.get());
    m_entities.push(Entity::new_soul(rb3));
    rb3->GetUserData().pointer = (uintptr_t)m_entities.last_mut().unwrap();

    auto chain = b2ChainShape();
    b2Vec2 verts[] = {
        b2Vec2(-480.f / 50.f, 200.f / 50.f),
        b2Vec2( 240.f / 50.f, 200.f / 50.f),
        b2Vec2( 240.f / 50.f, 180.f / 50.f),
        b2Vec2( 960.f / 50.f, 180.f / 50.f),
    };
    chain.CreateChain(verts, 4, b2Vec2(-500.f / 50.f, 200.f / 50.f), b2Vec2(980.f / 50.f, 180.f / 50.f));
    static_body(m_world.get(), b2Vec2(0.f, 0.f), &chain);

    auto ground = Sprite();
    ground.setPosition(-480, 200);
    ground.set_size(sf::Vector2f(1440, 50));
    ground.set_color(sf::Color(150, 0, 0));
    ground.set_depth(-10.f);
    m_environment.push(ground);
    ground.setPosition(240, 180);
    ground.set_size(sf::Vector2f(720, 20));
    ground.set_color(sf::Color(150, 0, 0));
    ground.set_depth(-10.f);
    m_environment.push(ground);

    // auto wall = Sprite();
    // wall.set_texture(*m_textures.get("empty").unwrap());
    // wall.set_depth(-11.f);
    // wall.setPosition(0, 0);
    // m_environment.push(wall);
    // wall.setPosition(480, 0);
    // m_environment.push(wall);
    // wall.setPosition(0, 0);
    // m_environment.push(wall);
    // wall.setPosition(-480, 0);
    // m_environment.push(wall);
}


void Level::load(const std::string& filename)
{
    auto src = fs::read_to_string(filename).unwrap();
    const auto doc = rustjson::Value::parse(src.data()).unwrap();

    for (const auto& tex : doc.get_array("textures"))
    {
        const auto& file = tex.get_str("file");
        const auto& name = tex.get_str("name");
        sf::Texture t;
        if(!t.loadFromFile(file)) {
            panic("falied to load texture '%s'", file.c_str());
        }
        m_textures.insert(name, t);
    }

    for (const auto& env : doc.get_array("world"))
    {
        const auto& tf = env.get("transform");

        Sprite sprite;
        sprite.setPosition(sf::Vector2f((f32)tf.get("loc").get_f64(0), (f32)tf.get("loc").get_f64(1)));
        sprite.setRotation((f32)tf.get_f64("rot"));
        sprite.setScale(sf::Vector2f((f32)tf.get("scale").get_f64(0), (f32)tf.get("scale").get_f64(1)));

        const auto& sp = env.get("sprite");

        if (!sp.get("texture").is_null()) {
            sprite.set_texture(*m_textures.get(sp.get_str("texture")).unwrap());
        }
        if (!sp.get("color").is_null()) {
            const auto& col = sp.get("color");
            sprite.set_color(sf::Color((u8)col.get_u64(0), (u8)col.get_u64(1), (u8)col.get_u64(2)));
        }
        if (!sp.get("anim").is_null()) {

        }
        if (!sp.get("size").is_null()) {
            const auto& sz = sp.get("size");
            sprite.set_size(sf::Vector2f((f32)sz.get_f64(0), (f32)sz.get_f64(1)));
        }
        sprite.set_depth((f32)sp.get_f64("depth"));

        m_environment.push(sprite);
    }
}


void Level::destroy_queued()
{
    auto freelist = Vec<usize>::make();
    for (usize i = 0; i < m_entities.len(); i++) {
        if (m_entities[i].destroy()) {
            freelist.push(i);
        }
    }
    for (isize i = freelist.len() - 1; i >= 0; i--) {
        usize j = freelist[i];
        if (m_entities[j].get_body()) {
            m_world->DestroyBody(m_entities[j].get_body());
        }
        m_entities.remove(j);
    }
}


void Level::handle_event(const sf::Event& event)
{
    for (auto& e : m_entities) {
        e.handle_event(event);
    }
}


std::ostream& operator<<(std::ostream& stream, sf::Vector2f vec) {
    return stream << '(' << vec.x << ", " << vec.y << ')';
}


void Level::update(const f32 dt)
{
    destroy_queued();

    m_world->Step(dt, 6, 2);

    for (auto& e : m_entities) {
        e.update(dt);
    }

    m_camera.update(dt);

#ifdef TESTING
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
        m_camera.set_mode(PlayerTracker{ p_player });
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
        m_camera.set_mode(FixedPosition{ sf::Vector2f(400.f, 50.f), 0.05f });
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
        m_camera.set_mode(FixedPosition{ sf::Vector2f(400.f, 50.f), 1.f });
    }
#endif
}


void Level::render(sf::RenderTarget& target) const
{
    target.setView(m_camera.get_view());

    auto sprites = Vec<const Sprite*>::with_capacity(m_entities.len() + m_environment.len());

    for (const auto& e : m_entities) {
        sprites.push(&e.get_sprite());
    }

    for (const auto& s : m_environment) {
        sprites.push(&s);
    }

    sprites.sort_by([](const Sprite* left, const Sprite* right){ return left->depth() < right->depth(); });

    for (auto s : sprites)
    {
        target.draw(*s);
    }

#ifdef TESTING
    // m_debugdraw.set_target(target);
    // m_world->DebugDraw();
#endif
}

