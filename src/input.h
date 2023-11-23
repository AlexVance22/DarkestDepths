#pragma once
#include "pch.h"


class Input
{
public:
    enum class Action {
        MoveLeft,
        MoveRight,
        Jump,
        Dash,

        BasicAttack,
        Block,
        HeavyAttack,
    };

    static void init();
    static void set_map(Action action, Input input);
    static Input get_map(Action action);
    static bool is_action_pressed(Action action);
    static Option<Action> from_event(const sf::Event& event);

private:
    static HashMap<Action, Input> s_maps;

public:
    enum class Kind {
        Key,
        Button,
    };

    Input(): m_value(), m_kind(Kind::Key) {}
    Input(i32 value, Kind kind): m_value(value), m_kind(kind) {}

private:
    i32 m_value;
    Kind m_kind;
};

