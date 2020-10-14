#include "MoneyDisplay.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>
#include <iomanip>
#include <sstream>

MoneyDisplay::MoneyDisplay(const sf::Font* font):
font(font) {
    textHeader.setCharacterSize(16);
    textHeader.setFont(*font);

    textBTC.setCharacterSize(12);
    textBTC.setFont(*font);
    textBTC.setFillColor({255, 255, 255, 192});

    textARS.setCharacterSize(24);
    textARS.setFont(*font);

    textUSD.setCharacterSize(12);
    textUSD.setFont(*font);
    textUSD.setFillColor({255, 255, 255, 192});

    rectangleHover.setFillColor({32, 32, 32});
}

void MoneyDisplay::setHeader(const sf::String& header) {
    textHeader.setString(header);
    dirty = true;
}

void MoneyDisplay::setHeaderColor(sf::Color bg, sf::Color text) {
    rectangleHeader.setFillColor(bg);
    textHeader.setFillColor(text);
}

void MoneyDisplay::setValues(double btc, double ars, double usd) {
    this->btc = btc;
    this->ars = ars;
    this->usd = usd;

    dirty = true;
}

void MoneyDisplay::setFactor(double factor) {
    this->factor = factor;
    dirty = true;
}

void MoneyDisplay::mouseMoved(sf::Vector2i mouse) {
    hovered = mouse.x >= position.x && mouse.y >= position.y && mouse.x < position.x + width && mouse.y <= position.y + 88;
}

void MoneyDisplay::setHovered(bool set) {
    hovered = set;
}

bool MoneyDisplay::isHovered() const {
    return hovered;
}

void MoneyDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (dirty) {
        dirty = false;
        sf::Vector2f center = position;
        center.x += width / 2.f;
        center.y = std::round(center.y);

        textHeader.setPosition({std::round(center.x - textHeader.getGlobalBounds().width / 2.f), center.y});

        rectangleHeader.setPosition(position + sf::Vector2f(2.f, 0));
        rectangleHeader.setSize({width - 4.f, 20.f});

        if (btc == 1.0 && factor == 1.0) {
            textBTC.setString("1 BTC");
        } else {
            std::stringstream ssBTC;
            ssBTC << std::fixed << std::setprecision(8) << std::fixed << btc * factor;
            textBTC.setString(ssBTC.str() + " BTC");
        }
        textBTC.setPosition({std::round(center.x - textBTC.getGlobalBounds().width / 2.f), center.y + 30.f});

        std::stringstream ssARS;
        ssARS << std::fixed << std::setprecision(2) << ars * factor;
        textARS.setString(ssARS.str() + " ARS");
        textARS.setPosition({std::round(center.x - textARS.getGlobalBounds().width / 2.f), center.y + 41.f});

        std::stringstream ssUSD;
        ssUSD << std::fixed << std::setprecision(2) << usd * factor;
        textUSD.setString(ssUSD.str() + " USD");
        textUSD.setPosition({std::round(center.x - textUSD.getGlobalBounds().width / 2.f), center.y + 66.f});

        rectangleHover.setPosition(position + sf::Vector2f(2.f, 20.f));
        rectangleHover.setSize({width - 4.f, 68.f});
    }

    if (hovered) {
        target.draw(rectangleHover, states);
    }

    target.draw(rectangleHeader, states);
    target.draw(textHeader, states);

    target.draw(textBTC, states);
    target.draw(textARS, states);
    target.draw(textUSD, states);
}
