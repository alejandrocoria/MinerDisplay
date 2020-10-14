#pragma once

#include "Scene.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>

class SceneMenu : public Scene {
public:
    SceneMenu(SceneManager* sceneManager, Scene* parent, sf::Vector2f position);
    ~SceneMenu() = default;

    void processEvents(sf::Event& event) override;
    void update(sf::Time time) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) override;
    void lostFocus() override;
    void gainFocus() override;
    void viewUpdated() override;

    void addItem(const sf::String& label);
    void setCallback(std::function<void(int)> fn);

private:
    void updateSizes();
    void updateSelection(sf::Vector2f mouse);

    Scene* parent;
    std::vector<sf::Text> items;
    sf::RectangleShape bg;
    sf::RectangleShape selection;
    std::function<void(int)> callback;
    int selected = -1;
    sf::Vector2f position;
};
