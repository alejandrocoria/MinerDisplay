#include "SceneDisplays.h"

#include "SceneManager.h"
#include "SceneMenu.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>

namespace {

const std::array<sf::Color, 12> colors {{
    {255, 0, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255}, {0, 0, 255}, {255, 0, 255}, {255, 127, 0}, {32, 144, 255}, {127, 0, 255}, {0, 127, 0}, {127, 64, 0}, {0, 127, 127}
}};

}


SceneDisplays::SceneDisplays(SceneManager* sceneManager):
Scene(sceneManager),
profitability(&sceneManager->getFont()),
balance(&sceneManager->getFont()),
value(&sceneManager->getFont()) {
    profitability.setFactor(1.f);
    profitability.setHeader(L"Rentabilidad / día");
    profitability.setHeaderColor({200, 200, 200, 185}, sf::Color::Black);
    balance.setHeader("Balance");
    balance.setHeaderColor({200, 200, 200, 255}, sf::Color::Black);
    value.setHeader("Value");
    value.setHeaderColor({200, 200, 200, 185}, sf::Color::Black);
}

void SceneDisplays::processEvents(sf::Event& event) {
    switch (event.type) {
        case sf::Event::Resized:
            updateSizes();
            break;
        case sf::Event::MouseLeft:
            profitability.setHovered(false);
            break;
        case sf::Event::MouseMoved:
            profitability.mouseMoved({event.mouseMove.x, event.mouseMove.y});
            break;
        case sf::Event::MouseButtonPressed:
            profitability.mouseMoved({event.mouseButton.x, event.mouseButton.y});
            break;
        case sf::Event::MouseButtonReleased:
            profitability.mouseMoved({event.mouseButton.x, event.mouseButton.y});
            if (event.mouseButton.button == sf::Mouse::Right && profitability.isHovered()) {
                std::unique_ptr<SceneMenu> menu = std::make_unique<SceneMenu>(sceneManager, this, sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                menu->addItem("Hora");
                menu->addItem(L"Día");
                menu->addItem("Mes");
                menu->addItem(L"Año");
                menu->setCallback([this](int s) {
                    if (s == 0) {
                        profitability.setHeader("Rentabilidad / hora");
                        profitability.setFactor(1.f / 24.f);
                    } else if (s == 1) {
                        profitability.setHeader(L"Rentabilidad / día");
                        profitability.setFactor(1.f);
                    } else if (s == 2) {
                        profitability.setHeader("Rentabilidad / mes");
                        profitability.setFactor(30.f);
                    } else {
                        profitability.setHeader(L"Rentabilidad / año");
                        profitability.setFactor(30.f * 12.f);
                    }
                });
                sceneManager->changeScene(std::move(menu), true);
            }
            break;
        default:
            break;
    }
}

void SceneDisplays::update(sf::Time time) {
    if (sceneManager->changesInRigData()) {
        rigs.clear();
        for (const RigJSON& json : sceneManager->getRigData().miningRigs) {
            rigs.emplace_back(&sceneManager->getFont(), json);
        }

        deviceCount = 0;
        for (RigDisplay& rig : rigs) {
            for (unsigned int i = 0; i < rig.getDeviceCount(); ++i) {
                rig.setDeviceColor(i, colors[deviceCount % colors.size()]);
                ++deviceCount;
            }
        }

        double profitabilityBTC = sceneManager->getRigData().totalProfitability;
        profitability.setValues(profitabilityBTC, sceneManager->BTCtoARS(profitabilityBTC), sceneManager->BTCtoUSD(profitabilityBTC));

        double balanceBTC = sceneManager->getBTCBalance();
        balance.setValues(balanceBTC, sceneManager->BTCtoARS(balanceBTC), sceneManager->BTCtoUSD(balanceBTC));

        value.setValues(1, sceneManager->BTCtoARS(1), sceneManager->BTCtoUSD(1));

        updateSizes();
    }
}

void SceneDisplays::draw(sf::RenderTarget& target, sf::RenderStates states) {
    target.draw(profitability, states);
    target.draw(balance, states);
    target.draw(value, states);

    for (const auto& rig : rigs) {
        target.draw(rig, states);
    }
}

void SceneDisplays::lostFocus() {

}

void SceneDisplays::gainFocus() {

}

void SceneDisplays::viewUpdated() {
    updateSizes();
}

void SceneDisplays::updateSizes() {
    const sf::View& view = sceneManager->getView();
    int deviceWidth = view.getSize().x / deviceCount;
    sf::Vector2f position = sf::Vector2f(0.f, view.getSize().y);
    for (auto& rig : rigs) {
        int rigWidth = rig.getDeviceCount() * deviceWidth;
        rig.setPosition(position);
        rig.setWidth(rigWidth);
        position.x += rigWidth;
    }

    int headerWidth = view.getSize().x / 3;

    profitability.setWidth(headerWidth);

    balance.setPosition({headerWidth, 0});
    balance.setWidth(headerWidth);

    value.setPosition({headerWidth * 2, 0});
    value.setWidth(headerWidth);
}
