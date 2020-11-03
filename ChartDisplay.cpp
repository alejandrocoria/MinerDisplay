#include "ChartDisplay.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <string>

namespace {
    const std::array<sf::String, 8> xLabelStrings = {
            L"Hoy", L"1 Día", L"2 Días", L"3 Días",
            L"4 Días", L"5 Días", L"6 Días", L"7 Días"};
}

ChartDisplay::ChartDisplay(const sf::Font* font):
font(font),
fromX(0),
toX(std::numeric_limits<unsigned long long>::max()),
line(sf::LineStrip),
axisLines(sf::LineStrip) {
    rectangle.setFillColor({32, 32, 32});

    clearPoints();
}

void ChartDisplay::setHeight(int height) {
    this->height = height;
    dirty = true;
}

void ChartDisplay::clearPoints() {
    points.clear();

    dirty = true;
}

void ChartDisplay::addPoint(Point point) {
    points.push_back(point);

    dirty = true;
}

void ChartDisplay::setXRange(unsigned long long from, unsigned long long to) {
    fromX = from;
    toX = to;

    dirty = true;
}

void ChartDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (dirty) {
        dirty = false;

        line.clear();
        xLabels.clear();
        yLabels.clear();
        axisLines.clear();

        if (!points.empty()) {
            auto comparePoints = [](const Point &a, const Point &b) {
                return a.x < b.x;
            };

            std::sort(points.begin(), points.end(), comparePoints);

            auto from = points.begin();
            auto to = points.end();

            if (points.front().x < fromX) {
                from = std::lower_bound(points.begin(), points.end(), Point{fromX, 0}, comparePoints);
            }

            if (points.back().x > toX) {
                to = std::upper_bound(points.begin(), points.end(), Point{fromX, 0}, comparePoints);
            }

            minimum.x = std::numeric_limits<unsigned long long>::max();
            maximum.x = std::numeric_limits<unsigned long long>::lowest();
            maximum.y = std::numeric_limits<double>::lowest();

            for (auto it = from; it != to; ++it) {
                if (it->x < minimum.x)
                    minimum.x = it->x;
                if (it->x > maximum.x)
                    maximum.x = it->x;
                if (it->y > maximum.y)
                    maximum.y = it->y;
            }

            int yCount = std::ceil((maximum.y + 25.0) / 100.0);
            maximum.y = yCount * 100.0;

            sf::Vector2f graphOrigin = position + sf::Vector2f(32.f, height - 16.f);
            sf::Vector2f graphSize = sf::Vector2f(width - 48.f, height - 28.f);
            float xExtent = static_cast<float>(maximum.x - minimum.x);
            float yExtent = maximum.y;

            for (auto it = from; it != to; ++it) {
                float xPos = (it->x - minimum.x) / xExtent * graphSize.x + graphOrigin.x;
                float yPos = it->y / yExtent * -graphSize.y + graphOrigin.y;
                line.append(sf::Vertex({xPos, yPos}));
            }

            for (int i = 0; i <= 7; ++i) {
                sf::Text text;
                text.setFont(*font);
                text.setCharacterSize(10);
                text.setFillColor({255, 255, 255, 196});
                text.setString(xLabelStrings[i]);
                float xPos = graphOrigin.x + graphSize.x;
                xPos -= i * (graphSize.x / 7);
                text.setPosition(xPos, graphOrigin.y);
                text.setOrigin(std::round(text.getGlobalBounds().width / 2), 0.f);
                yLabels.push_back(text);
            }

            for (int i = 1; i <= yCount; ++i) {
                sf::Text text;
                text.setFont(*font);
                text.setCharacterSize(10);
                text.setFillColor({255, 255, 255, 196});
                text.setString(std::to_string(i * 100));
                float yPos = graphOrigin.y;
                yPos -= i * (graphSize.y / yCount);
                text.setPosition(graphOrigin.x - 4.f, yPos);
                text.setOrigin(text.getGlobalBounds().width, 6.f);
                yLabels.push_back(text);
            }

            axisLines.append({graphOrigin + sf::Vector2f(0.f, -graphSize.y)});
            axisLines.append({graphOrigin});
            axisLines.append({graphOrigin + sf::Vector2f(graphSize.x, 0.f)});
        }

        rectangle.setPosition(position + sf::Vector2f(2.f, 2.f));
        rectangle.setSize({width - 4.f, height - 4.f});
    }

    target.draw(rectangle, states);

    auto newStates = states;
    newStates.transform.translate(0.5f, 0.5f);
    target.draw(line, newStates);
    target.draw(axisLines, newStates);

    for (const sf::Text& text : yLabels) {
        target.draw(text, states);
    }
}
