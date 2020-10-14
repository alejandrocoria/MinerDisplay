#include "SceneSettings.h"

#include "SceneManager.h"
#include <SFML/Window/Event.hpp>

SceneSettings::SceneSettings(SceneManager* sceneManager):
Scene(sceneManager, true),
organizationID(&sceneManager->getFontMono()),
apiKey(&sceneManager->getFontMono()),
apiSecretKey(&sceneManager->getFontMono()),
cancel(&sceneManager->getFont()),
accept(&sceneManager->getFont()) {
    bg1.setFillColor({0, 0, 0, 127});
    bg2.setFillColor({32, 32, 32});
    bg2.setSize({520.f, 160.f});

    organizationIDLabel.setFont(sceneManager->getFont());
    organizationIDLabel.setCharacterSize(16);
    organizationIDLabel.setString("Organization");

    apiKeyLabel.setFont(sceneManager->getFont());
    apiKeyLabel.setCharacterSize(16);
    apiKeyLabel.setString("API Key");

    apiSecretKeyLabel.setFont(sceneManager->getFont());
    apiSecretKeyLabel.setCharacterSize(16);
    apiSecretKeyLabel.setString("API Secret Key");

    organizationID.setWidth(360);
    organizationID.setString(sceneManager->getSettings().organizationID);
    apiKey.setWidth(360);
    apiKey.setString(sceneManager->getSettings().apiKey);
    apiSecretKey.setWidth(360);
    apiSecretKey.setString(sceneManager->getSettings().apiSecretKey);
    apiSecretKey.setCursorPos(0);

    accept.setText("Aceptar");
    accept.setColors({64, 64, 64}, {128, 128, 128}, {48, 48, 48}, sf::Color::White);
    accept.setSize({92.f, 30.f});
    accept.setCallback([this,sceneManager]{changeSettings(); sceneManager->popBack();});

    cancel.setText("Cancelar");
    cancel.setColors({64, 64, 64}, {128, 128, 128}, {48, 48, 48}, sf::Color::White);
    cancel.setSize({92.f, 30.f});
    cancel.setCallback([sceneManager]{sceneManager->popBack();});

    updateSizes();
}

void SceneSettings::processEvents(sf::Event& event) {
    organizationID.processEvents(event);
    apiKey.processEvents(event);
    apiSecretKey.processEvents(event);
    accept.processEvents(event);
    cancel.processEvents(event);

    switch (event.type) {
        case sf::Event::Resized:
            updateSizes();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape) {
                sceneManager->popBack();
            }
            break;
        default:
            break;
    }
}

void SceneSettings::update(sf::Time time) {
    organizationID.update(time);
    apiKey.update(time);
    apiSecretKey.update(time);
}

void SceneSettings::draw(sf::RenderTarget& target, sf::RenderStates states) {
    target.draw(bg1, states);
    target.draw(bg2, states);
    target.draw(organizationIDLabel, states);
    target.draw(apiKeyLabel, states);
    target.draw(apiSecretKeyLabel, states);
    target.draw(organizationID, states);
    target.draw(apiKey, states);
    target.draw(apiSecretKey, states);
    target.draw(accept, states);
    target.draw(cancel, states);
}

void SceneSettings::lostFocus() {

}

void SceneSettings::gainFocus() {

}

void SceneSettings::viewUpdated() {
    updateSizes();
}

void SceneSettings::updateSizes() {
    const sf::View& view = sceneManager->getView();
    sf::Vector2f size = view.getSize();
    sf::Vector2f center = size / 2.f;

    bg1.setSize(size);

    bg2.setPosition(center + sf::Vector2f(-260.f, -80.f));

    organizationIDLabel.setPosition(center + sf::Vector2f(-240.f, -60.f));
    apiKeyLabel.setPosition(center + sf::Vector2f(-240.f, -34.f));
    apiSecretKeyLabel.setPosition(center + sf::Vector2f(-240.f, -8.f));

    organizationID.setPosition(center + sf::Vector2f(-120.f, -60.f));
    apiKey.setPosition(center + sf::Vector2f(-120.f, -34.f));
    apiSecretKey.setPosition(center + sf::Vector2f(-120.f, -8.f));

    accept.setPosition(center + sf::Vector2f(150.f, 32.f));
    cancel.setPosition(center + sf::Vector2f(40.f, 32.f));
}

void SceneSettings::changeSettings() {
    Settings settings;
    settings.organizationID = organizationID.getString();
    settings.apiKey = apiKey.getString();
    settings.apiSecretKey = apiSecretKey.getString();

    sceneManager->setSettings(settings);
}
