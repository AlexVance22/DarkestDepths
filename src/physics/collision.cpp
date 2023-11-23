#include "pch.h"
#include "collision.h"

#include "entity.h"


void ContactListener::BeginContact(b2Contact* contact) {
    auto f1 = contact->GetFixtureA();
    auto f2 = contact->GetFixtureB();
    if (auto e = (Entity*)f1->GetBody()->GetUserData().pointer)
        e->collide(f1, f2);
    if (auto e = (Entity*)f2->GetBody()->GetUserData().pointer)
        e->collide(f2, f1);
}

void ContactListener::EndContact(b2Contact* contact) {
    auto f1 = contact->GetFixtureA();
    auto f2 = contact->GetFixtureB();
    if (auto e = (Entity*)f1->GetBody()->GetUserData().pointer)
        e->decollide(f1, f2);
    if (auto e = (Entity*)f2->GetBody()->GetUserData().pointer)
        e->decollide(f2, f1);
}

