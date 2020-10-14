#pragma once

#include <SFML/System/Vector2.hpp>

class BaseDisplay {
public:
    virtual ~BaseDisplay() = default;

    virtual void setPosition(sf::Vector2f position) {this->position = position; dirty = true;}
    virtual void setWidth(int width) {this->width = width; dirty = true;}

protected:
    mutable bool dirty = true;
    sf::Vector2f position;
    int width = 0;
};
