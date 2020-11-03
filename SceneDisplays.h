#pragma once

#include "ChartDisplay.h"
#include "RigDisplay.h"
#include "MoneyDisplay.h"
#include "Scene.h"
#include <SFML/Graphics/Text.hpp>

class SceneDisplays : public Scene {
public:
    SceneDisplays(SceneManager* sceneManager);
    ~SceneDisplays() = default;

    void processEvents(sf::Event& event) override;
    void update(sf::Time time) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) override;
    void lostFocus() override;
    void gainFocus() override;
    void viewUpdated() override;

private:
    void updateSizes();

    MoneyDisplay profitability;
    MoneyDisplay balance;
    MoneyDisplay value;
    std::vector<RigDisplay> rigs;
    ChartDisplay algoChart;
    unsigned int deviceCount = 0;
};
