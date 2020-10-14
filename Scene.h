#pragma once

#include <SFML/System/NonCopyable.hpp>

namespace sf {
class Event;
class Time;
class RenderTarget;
class RenderStates;
}

class SceneManager;

class Scene : sf::NonCopyable {
public:
    Scene(SceneManager* sceneManager, bool popup = false, bool keyRepeat = true):
        sceneManager(sceneManager), popup(popup), keyRepeat(keyRepeat) {}
    virtual ~Scene() = default;

    virtual void processEvents(sf::Event& event) = 0;
    virtual void update(sf::Time time) = 0;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) = 0;
    virtual void lostFocus() {}
    virtual void gainFocus() {}
    virtual void viewUpdated() {}

    bool isPopup() const {return popup;}
    bool isKeyRepeat() const {return keyRepeat;}

protected:
    SceneManager* sceneManager;
    bool popup;
    bool keyRepeat;
};
