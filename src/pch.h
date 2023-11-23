#pragma once

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <box2d/box2d.h>

#include <rusty/core.h>
#include <rusty/collections.h>
#include <rusty/fs.h>
#include <rusty/json.h>

#include <iostream>

#define vec(T, ...) Vec<T>(std::vector<T>{ __VA_ARGS__ })
#define Variant(name, ...) using name = std::variant<__VA_ARGS__>
#define Enum(name, ...) struct name { using Value_t = std::variant<__VA_ARGS__>; Value_t data; enum { __VA_ARGS__ } kind; }

using namespace core;
using collections::HashMap;
using collections::HashSet;

