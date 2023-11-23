#include "pch.h"
#include "input.h"


HashMap<Input::Action, Input> Input::s_maps;


void Input::init() {
    s_maps.insert(Input::Action::MoveLeft,    Input((i32)sf::Keyboard::Key::A,        Kind::Key));
    s_maps.insert(Input::Action::MoveRight,   Input((i32)sf::Keyboard::Key::D,        Kind::Key));
    s_maps.insert(Input::Action::Jump,        Input((i32)sf::Keyboard::Key::Space,    Kind::Key));
    s_maps.insert(Input::Action::Dash,        Input((i32)sf::Keyboard::Key::LShift,   Kind::Key));
    s_maps.insert(Input::Action::BasicAttack, Input((i32)sf::Mouse::Button::Left,     Kind::Button));
    s_maps.insert(Input::Action::Block,       Input((i32)sf::Mouse::Button::Right,    Kind::Button));
    s_maps.insert(Input::Action::HeavyAttack, Input((i32)sf::Mouse::Button::XButton1, Kind::Button));
}

void Input::set_map(Action action, Input key) {
    s_maps.insert(action, key);
}

Input Input::get_map(Action action) {
    return *s_maps.get(action).unwrap();
}

bool Input::is_action_pressed(Action action) {
    auto in = s_maps.get(action).unwrap();

    if (in->m_kind == Kind::Key) {
        return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)in->m_value);
    } else if (in->m_kind == Kind::Button) {
        return sf::Mouse::isButtonPressed((sf::Mouse::Button)in->m_value);
    }

    return false;
}

Option<Input::Action> Input::from_event(const sf::Event& event) {
    switch (event.type)
    {
    case sf::Event::MouseButtonPressed:
        for (const auto& [ac, in] : s_maps) {
            if (in.m_kind == Kind::Button) {
                if (in.m_value == event.mouseButton.button) {
                    return Option<Input::Action>::Some(ac);
                }
            }
        }
        break;
    case sf::Event::KeyPressed:
        for (const auto& [ac, in] : s_maps) {
            if (in.m_kind == Kind::Key) {
                if (in.m_value == event.key.code) {
                    return Option<Input::Action>::Some(ac);
                }
            }
        }
        break;
    default:
        break;
    }

    return Option<Input::Action>::None();
}

