#include "SceneMenu.h"

#include "SceneManager.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

SceneMenu::SceneMenu(SceneManager* sceneManager, Scene* parent, sf::Vector2f position):
Scene(sceneManager, true),
parent(parent),
position(position) {
    bg.setFillColor({64, 64, 64});
    selection.setFillColor({128, 128, 128});

    updateSizes();
}

void SceneMenu::processEvents(sf::Event& event) {
    switch (event.type) {
        case sf::Event::Resized:
            sceneManager->popBack();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape) {
                sceneManager->popBack();
            }
            break;
        case sf::Event::MouseMoved:
            updateSelection({event.mouseMove.x, event.mouseMove.y});
            break;
        case sf::Event::MouseButtonPressed:
            updateSelection({event.mouseButton.x, event.mouseButton.y});

            if (event.mouseButton.button == sf::Mouse::Left) {
                if (selected >= 0 && parent) {
                    parent->processEvents(event);
                }

                if (selected >= 0 && callback) {
                    callback(selected);
                }
                sceneManager->popBack();
            } else if (selected < 0) {
                sceneManager->popBack();
            }
            break;
        case sf::Event::MouseButtonReleased:
            updateSelection({event.mouseButton.x, event.mouseButton.y});
            break;
        default:
            break;
    }
}

void SceneMenu::update(sf::Time time) {

}

void SceneMenu::draw(sf::RenderTarget& target, sf::RenderStates states) {
    target.draw(bg, states);

    if (selected >= 0) {
        target.draw(selection, states);
    }

    for (const sf::Text& item : items) {
        target.draw(item, states);
    }
}

void SceneMenu::lostFocus() {

}

void SceneMenu::gainFocus() {

}

void SceneMenu::viewUpdated() {
    sceneManager->popBack();
}

void SceneMenu::addItem(const sf::String& label) {
    items.emplace_back(label, sceneManager->getFont(), 18);

    updateSizes();
}

void SceneMenu::setCallback(std::function<void(int)> fn) {
    callback = std::move(fn);
}

void SceneMenu::updateSizes() {
    sf::Vector2f topLeft = position + sf::Vector2f(8, 4);
    float maxWidth = 16;
    for (sf::Text& item : items) {
        item.setPosition(topLeft);
        topLeft.y += 22;

        float width = item.getGlobalBounds().width;
        if (width > maxWidth) {
            maxWidth = width;
        }
    }

    bg.setPosition(position);
    bg.setSize({maxWidth + 20, items.size() * 22 + 8});

    selection.setSize({maxWidth + 20, 22});
}

void SceneMenu::updateSelection(sf::Vector2f mouse) {
    if (bg.getGlobalBounds().contains(mouse)) {
        selected = (mouse.y - position.y - 4) / 22;
        if (selected >= items.size()) {
            selected = -1;
        }
    } else {
        selected = -1;
    }

    if (selected >= 0) {
        selection.setPosition(position.x, position.y + 4 + selected * 22);
    }
}
