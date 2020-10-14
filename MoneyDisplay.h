#pragma once

#include "BaseDisplay.h"
#include "RigsJSON.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>

class MoneyDisplay : public BaseDisplay, public sf::Drawable {
public:
    MoneyDisplay(const sf::Font* font);

    void setHeader(const sf::String& header);
    void setHeaderColor(sf::Color bg, sf::Color text);
    void setValues(double btc, double ars, double usd);
    void setFactor(double factor);
    void mouseMoved(sf::Vector2i mouse);
    void setHovered(bool set);
    bool isHovered() const;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    double btc = 0.0;
    double ars = 0.0;
    double usd = 0.0;
    double factor = 1.0;
    const sf::Font* font;
    mutable sf::Text textHeader;
    mutable sf::Text textBTC;
    mutable sf::Text textARS;
    mutable sf::Text textUSD;
    mutable sf::RectangleShape rectangleHeader;
    mutable sf::RectangleShape rectangleHover;
    bool hovered = false;
};
