#pragma once
#include <memory>

#include "rules.hpp"

class World {
public:
    virtual void addRule(std::unique_ptr<Rule> rule) = 0;
};