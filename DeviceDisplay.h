#pragma once

#include "BaseDisplay.h"
#include "RigsJSON.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>

class DeviceDisplay : public BaseDisplay, public sf::Drawable {
public:
    DeviceDisplay(const sf::Font* font, const DeviceJSON& deviceJSON);

    void setColor(sf::Color color);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::String id;
    sf::String name;
    int revolutionsPerMinute = -1;
    int load = -1;
    int temperature = -1;
    sf::String status;
    sf::String algoDisplaySuffix;
    sf::String algoSpeed;
    sf::String algoTitle;
    sf::String algorithm;

    const sf::Font* font;
    mutable sf::Text textStats;
    mutable sf::Text textSpeed;
    mutable sf::Text textAlgo;
    mutable sf::RectangleShape rectangle;
};
