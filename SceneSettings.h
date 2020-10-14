#pragma once

#include "Button.h"
#include "Scene.h"
#include "TextBox.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>

class SceneSettings : public Scene {
public:
    SceneSettings(SceneManager* sceneManager);
    ~SceneSettings() = default;

    void processEvents(sf::Event& event) override;
    void update(sf::Time time) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) override;
    void lostFocus() override;
    void gainFocus() override;
    void viewUpdated() override;

private:
    void updateSizes();
    void changeSettings();

    sf::RectangleShape bg1;
    sf::RectangleShape bg2;
    sf::Text organizationIDLabel;
    sf::Text apiKeyLabel;
    sf::Text apiSecretKeyLabel;
    TextBox organizationID;
    TextBox apiKey;
    TextBox apiSecretKey;
    Button cancel;
    Button accept;
};
