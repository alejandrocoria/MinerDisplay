#include "RigDisplay.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <cassert>

RigDisplay::RigDisplay(const sf::Font* font, const RigJSON& rigJSON):
font(font),
bg(sf::TriangleStrip, 4) {
    id = rigJSON.rigId;
    name = rigJSON.name;

    for (const DeviceJSON& json : rigJSON.devices) {
        devices.emplace_back(font, json);
    }

    textName.setCharacterSize(16);
    textName.setFont(*font);
    textName.setString(name);
    textName.setFillColor(sf::Color::Black);

    bg[0].color = sf::Color(200, 200, 200, 255);
    bg[1].color = sf::Color(200, 200, 200, 255);
    bg[2].color = sf::Color(200, 200, 200, 185);
    bg[3].color = sf::Color(200, 200, 200, 185);
}

unsigned int RigDisplay::getDeviceCount() const {
    return devices.size();
}

void RigDisplay::setDeviceColor(unsigned int index, sf::Color color) {
    assert(index < devices.size());
    devices[index].setColor(color);
}

void RigDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (dirty) {
        dirty = false;

        sf::Vector2f devicePosition = position + sf::Vector2f(0.f, -20.f);
        int deviceWidth = width / devices.size();
        for (auto& device : devices) {
            device.setPosition(devicePosition);
            device.setWidth(deviceWidth);
            devicePosition.x += deviceWidth;
        }

        textName.setPosition(position + sf::Vector2f(6.f, -20.f));

        bg[0].position = position + sf::Vector2f(2.f, -20.f);
        bg[1].position = position + sf::Vector2f(2.f, 0.f);
        bg[2].position = position + sf::Vector2f(width - 2.f, -20.f);
        bg[3].position = position + sf::Vector2f(width - 2.f, 0.f);
    }

    target.draw(bg, states);
    target.draw(textName, states);

    for (const auto& device : devices) {
        target.draw(device, states);
    }
}
