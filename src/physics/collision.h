#pragma once
#include "pch.h"


class ContactListener : public b2ContactListener {
private:

public:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};

