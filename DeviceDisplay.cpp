#include "DeviceDisplay.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

DeviceDisplay::DeviceDisplay(const sf::Font* font, const DeviceJSON& deviceJSON):
font(font) {
    id = deviceJSON.id;
    name = deviceJSON.name;
    revolutionsPerMinute = deviceJSON.revolutionsPerMinute;
    load = deviceJSON.load;
    temperature = deviceJSON.temperature;
    status = deviceJSON.status.enumName;

    if (deviceJSON.speeds.size() > 0) {
        const SpeedJSON& speed = deviceJSON.speeds[0];
        algoDisplaySuffix = speed.displaySuffix;
        algoSpeed = speed.speed;
        algoTitle = speed.title;
        algorithm = speed.algorithm;
    }

    sf::String stats;
    stats += std::to_string(revolutionsPerMinute) + " RPM   ";
    //stats += std::to_string(load) + "%  ";
    stats += std::to_string(temperature);
    stats += L" ºC";

    textStats.setCharacterSize(16);
    textStats.setFont(*font);
    textStats.setString(stats);

    sf::String speed;
    if (status == "MINING") {
        std::size_t dotPos = algoSpeed.find(".");
        if (dotPos != sf::String::InvalidPos) {
            algoSpeed = algoSpeed.substring(0, dotPos + 3);
        }
        speed += algoSpeed + " " + algoDisplaySuffix + "/s";
    } else {
        speed += status;
    }

    textSpeed.setCharacterSize(16);
    textSpeed.setFont(*font);
    textSpeed.setString(speed);

    if (status == "MINING") {
        textSpeed.setFillColor({255, 255, 255});
    } else if (status == "BENCHMARKING") {
        textSpeed.setFillColor({255, 192, 32});
    } else if (status == "ERROR") {
        textSpeed.setFillColor({255, 32, 32});
    } else {
        textSpeed.setFillColor({255, 255, 255, 127});
    }

    sf::String algo;
    if (deviceJSON.deviceType.enumName == "CPU") {
        algo = "CPU";
    } else if (deviceJSON.deviceType.enumName != "UNKNOWN") {
        algo = "GPU";
    }

    if (status == "MINING") {
        if (deviceJSON.deviceType.enumName != "UNKNOWN") {
            algo += " - ";
        }
        algo += algoTitle;
    }

    textAlgo.setCharacterSize(12);
    textAlgo.setFont(*font);
    textAlgo.setString(algo);
    textAlgo.setFillColor({255, 255, 255, 127});
}

void DeviceDisplay::setColor(sf::Color color) {
    rectangle.setFillColor(color);
}

void DeviceDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (dirty) {
        dirty = false;
        sf::Vector2f center = position;
        center.x += width / 2.f;
        center.y = std::round(center.y);

        textStats.setPosition({std::round(center.x - textStats.getGlobalBounds().width / 2.f), center.y - 67.f});
        textSpeed.setPosition({std::round(center.x - textSpeed.getGlobalBounds().width / 2.f), center.y - 46.f});
        textAlgo.setPosition({std::round(center.x - textAlgo.getGlobalBounds().width / 2.f), center.y - 25.f});

        rectangle.setPosition(position + sf::Vector2f(4.f, -4.f));
        rectangle.setSize({width - 8.f, 4.f});
    }

    target.draw(rectangle, states);
    target.draw(textStats, states);
    target.draw(textSpeed, states);
    target.draw(textAlgo, states);
}
