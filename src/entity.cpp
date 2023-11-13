#include <entity.h>


Entity::Entity()
{
    movement = Movement::soul({ 5.f});
}

void Entity::update(const f32 dt)
{
    sprite.update_anim(dt);
}

