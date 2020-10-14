#pragma once

#include "BaseDisplay.h"
#include "DeviceDisplay.h"
#include "RigsJSON.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>

class RigDisplay : public BaseDisplay, public sf::Drawable {
public:
    RigDisplay(const sf::Font* font, const RigJSON& rigJSON);

    unsigned int getDeviceCount() const;
    void setDeviceColor(unsigned int index, sf::Color color);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::String id;
    sf::String name;
    const sf::Font* font;
    mutable std::vector<DeviceDisplay> devices;
    mutable sf::Text textName;
    mutable sf::VertexArray bg;
};
