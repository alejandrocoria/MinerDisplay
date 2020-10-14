#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

class TextBox : public sf::Drawable {
public:
    TextBox(const sf::Font* font);

    void processEvents(const sf::Event& event);
    void update(sf::Time time);
    void setString(const sf::String& string);
    const sf::String& getString() const;
    void setPosition(sf::Vector2f position);
    void setWidth(int width);
    void setCursorPos(int cursorPos, bool selecting = false);
    void setSelected(bool set);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void mouseEvent(int x);
    void removeSelected();
    void updateDrawOffset();

    const sf::Font* font;
    bool selected = false;
    sf::String string;
    sf::Time elapsed;
    int startSelection = -1;
    bool mouseSelecting = false;
    int cursor = 0;
    mutable sf::Text text;
    mutable sf::RectangleShape rectangle;
    mutable sf::RectangleShape selectionRect;
    mutable sf::VertexArray cursorLine;
    mutable sf::VertexArray fadeLines;
    mutable sf::View view;
    mutable bool dirty = true;
    sf::Vector2f position;
    int width = 0;
    int drawOffset = -2.f;
};
