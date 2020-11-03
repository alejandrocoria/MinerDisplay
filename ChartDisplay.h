#pragma once

#include "BaseDisplay.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>

class ChartDisplay : public BaseDisplay, public sf::Drawable {
public:
    struct Point {
        unsigned long long x;
        double y;
    };

    ChartDisplay(const sf::Font* font);
    void setHeight(int height);
    void clearPoints();
    void addPoint(Point point);
    void setXRange(unsigned long long from, unsigned long long to);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    const sf::Font* font;
    unsigned long long fromX;
    unsigned long long toX;
    mutable std::vector<Point> points;
    mutable Point minimum;
    mutable Point maximum;
    mutable sf::VertexArray line;
    mutable std::vector<sf::Text> xLabels;
    mutable std::vector<sf::Text> yLabels;
    mutable sf::VertexArray axisLines;
    mutable sf::RectangleShape rectangle;
    int height = 0;
};
