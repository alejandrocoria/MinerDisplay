#include "Button.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

Button::Button(const sf::Font* font):
font(font) {
    text.setCharacterSize(16);
    text.setFont(*font);
}

void Button::processEvents(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (rectangle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                setSate(State::Pressed);
            } else {
                setSate(State::Normal);
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (rectangle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                if (state == State::Pressed) {
                    setSate(State::Hovered);
                    if (callback) {
                        callback();
                    }
                }
            } else {
                setSate(State::Normal);
            }
            break;
        case sf::Event::MouseMoved:
            if (rectangle.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                if (state == State::Normal) {
                    setSate(State::Hovered);
                }
            } else {
                if (state == State::Hovered) {
                    setSate(State::Normal);
                }
            }
            break;
        default:
            break;

    }
}

void Button::setText(const sf::String& text) {
    this->text.setString(text);
    dirty = true;
}

void Button::setColors(sf::Color normal, sf::Color hovered, sf::Color pressed, sf::Color text) {
    this->normal = normal;
    this->hovered = hovered;
    this->pressed = pressed;
    this->text.setFillColor(text);

    updateRectangleColor();
}

void Button::setSize(sf::Vector2f size) {
    this->size = size;
    dirty = true;
}

void Button::setPosition(sf::Vector2f position) {
    this->position = position;
    dirty = true;
}

void Button::setCallback(std::function<void()> fn) {
    callback = std::move(fn);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (dirty) {
        dirty = false;
        sf::Vector2f center = position + size / 2.f;

        rectangle.setPosition(position);
        rectangle.setSize(size);

        text.setPosition({std::round(center.x - text.getGlobalBounds().width / 2.f), std::round(center.y - 10.f)});
    }

    target.draw(rectangle, states);
    target.draw(text, states);
}

void Button::setSate(State newState) {
    if (newState == state) {
        return;
    }

    state = newState;
    updateRectangleColor();
}

void Button::updateRectangleColor() {
    if (state == State::Normal) {
        rectangle.setFillColor(normal);
    } else if (state == State::Hovered) {
        rectangle.setFillColor(hovered);
    } else if (state == State::Pressed) {
        rectangle.setFillColor(pressed);
    }
}
