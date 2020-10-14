#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>

class Button : public sf::Drawable {
public:
    Button(const sf::Font* font);

    void processEvents(const sf::Event& event);
    void setText(const sf::String& text);
    void setColors(sf::Color normal, sf::Color hovered, sf::Color pressed, sf::Color text);
    void setSize(sf::Vector2f size);
    void setPosition(sf::Vector2f position);
    void setCallback(std::function<void()> fn);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    enum class State {Normal, Hovered, Pressed};

    void setSate(State newState);
    void updateRectangleColor();

    const sf::Font* font;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Color normal;
    sf::Color hovered;
    sf::Color pressed;
    mutable sf::Text text;
    mutable sf::RectangleShape rectangle;
    mutable bool dirty = false;
    State state = State::Normal;
    std::function<void()> callback;
};
