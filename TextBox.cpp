#include "TextBox.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <cmath>

TextBox::TextBox(const sf::Font* font):
font(font),
cursorLine(sf::Lines, 2),
fadeLines(sf::Lines, 8) {
    text.setCharacterSize(16);
    text.setFont(*font);

    rectangle.setFillColor(sf::Color::Black);
    rectangle.setOutlineColor({64, 64, 64});
    rectangle.setOutlineThickness(2);

    selectionRect.setFillColor({64, 64, 64});

    fadeLines[0].color = sf::Color(0, 0, 0, 127); fadeLines[1].color = sf::Color(0, 0, 0, 127);
    fadeLines[2].color = sf::Color(0, 0, 0,  64); fadeLines[3].color = sf::Color(0, 0, 0,  64);
    fadeLines[4].color = sf::Color(0, 0, 0,  64); fadeLines[5].color = sf::Color(0, 0, 0,  64);
    fadeLines[6].color = sf::Color(0, 0, 0, 127); fadeLines[7].color = sf::Color(0, 0, 0, 127);
}

void TextBox::processEvents(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::TextEntered:
            if (selected && event.text.unicode >= 32) {
                if (startSelection != -1) {
                    removeSelected();
                }

                string.insert(cursor, event.text.unicode);
                ++cursor;
                text.setString(string);
                dirty = true;
                elapsed = sf::Time::Zero;
                updateDrawOffset();
            }
            break;
        case sf::Event::KeyPressed:
            if (selected) {
                if (event.key.code == sf::Keyboard::Backspace) {
                    if (startSelection != -1) {
                        removeSelected();
                    } else if (cursor > 0) {
                        --cursor;
                        string.erase(cursor);
                        text.setString(string);
                        elapsed = sf::Time::Zero;
                        dirty = true;
                        updateDrawOffset();
                    }
                } else if (event.key.code == sf::Keyboard::Delete) {
                    if (startSelection != -1) {
                        removeSelected();
                    } else if (cursor < string.getSize()) {
                        string.erase(cursor);
                        text.setString(string);
                        elapsed = sf::Time::Zero;
                        dirty = true;
                        updateDrawOffset();
                    }
                } else if (event.key.code == sf::Keyboard::Left) {
                    setCursorPos(cursor - 1, event.key.shift);
                } else if (event.key.code == sf::Keyboard::Right) {
                    setCursorPos(cursor + 1, event.key.shift);
                } else if (event.key.code == sf::Keyboard::Home) {
                    setCursorPos(0, event.key.shift);
                } else if (event.key.code == sf::Keyboard::End) {
                    setCursorPos(string.getSize(), event.key.shift);
                } else if (event.key.code == sf::Keyboard::V && event.key.control) {
                    if (startSelection != -1) {
                        removeSelected();
                    }

                    sf::String c = sf::Clipboard::getString();
                    string.insert(cursor, c);
                    cursor += c.getSize();
                    text.setString(string);
                    elapsed = sf::Time::Zero;
                    dirty = true;
                    updateDrawOffset();
                } else if ((event.key.code == sf::Keyboard::C || event.key.code == sf::Keyboard::X) && event.key.control && startSelection != -1) {
                    sf::String c = string.substring(std::min(cursor, startSelection), std::abs(cursor - startSelection));
                    sf::Clipboard::setString(c);

                    if (event.key.code == sf::Keyboard::X) {
                        removeSelected();
                        text.setString(string);
                        elapsed = sf::Time::Zero;
                        dirty = true;
                        updateDrawOffset();
                    }
                }
            }
            break;
        case sf::Event::MouseButtonPressed:
            if (rectangle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                mouseEvent(event.mouseButton.x);
                if (!mouseSelecting) {
                    startSelection = cursor;
                    mouseSelecting = true;
                }
            } else {
                selected = false;
                startSelection = -1;
                mouseSelecting = false;
                dirty = true;
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (mouseSelecting) {
                mouseEvent(event.mouseButton.x);
                mouseSelecting = false;
                if (startSelection == cursor) {
                    startSelection = -1;
                }
            }
            break;
        case sf::Event::MouseMoved:
            if (mouseSelecting) {
                mouseEvent(event.mouseMove.x);
            }
            break;
        default:
            break;

    }
}

void TextBox::update(sf::Time time) {
    elapsed += time;
    if (elapsed >= sf::seconds(1.f)) {
        elapsed = sf::Time::Zero;
    }
}

void TextBox::setString(const sf::String& string) {
    this->string = string;
    text.setString(string);
    cursor = string.getSize();
    dirty = true;
    updateDrawOffset();
}

const sf::String& TextBox::getString() const {
    return string;
}

void TextBox::setPosition(sf::Vector2f position) {
    this->position = position;
    dirty = true;
}

void TextBox::setWidth(int width) {
    this->width = width;
    dirty = true;
    updateDrawOffset();
}

void TextBox::setCursorPos(int cursorPos, bool selecting) {
    if (cursorPos >= 0 && cursorPos <= string.getSize()) {
        if (selecting) {
            if (startSelection == -1) {
                startSelection = cursor;
            }
        } else {
            startSelection = -1;
        }

        cursor = cursorPos;
        dirty = true;
        updateDrawOffset();
    } else if (!selecting && startSelection != -1) {
        startSelection = -1;
        dirty = true;
    }

    elapsed = sf::Time::Zero;
}

void TextBox::setSelected(bool set) {
    if (set == selected) {
        return;
    }

    selected = set;
    dirty = true;

    if (!selected) {
        startSelection = -1;
        mouseSelecting = false;
    }
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::View oldView = target.getView();

    if (dirty) {
        dirty = false;

        text.setPosition(position + sf::Vector2f(1.f, 1.f));

        rectangle.setPosition(position);
        rectangle.setSize({width, 20.f});

        sf::Vector2f linePos = text.findCharacterPos(cursor) + sf::Vector2f(0.f, 2.f);
        cursorLine[0].position = linePos;
        cursorLine[1].position = linePos + sf::Vector2f(0.f, 14.f);

        if (startSelection != -1 && startSelection != cursor) {
            float startSelectionPos = text.findCharacterPos(startSelection).x;
            selectionRect.setPosition(sf::Vector2f(startSelectionPos, position.y + 1.f));
            selectionRect.setSize({linePos.x - startSelectionPos, 18.f});
        }

        fadeLines[0].position = position;
        fadeLines[1].position = position + sf::Vector2f(0.f, 20.f);
        fadeLines[2].position = position + sf::Vector2f(1.f, 0.f);
        fadeLines[3].position = position + sf::Vector2f(1.f, 20.f);
        fadeLines[4].position = position + sf::Vector2f(width - 1.f, 0.f);
        fadeLines[5].position = position + sf::Vector2f(width - 1.f, 20.f);
        fadeLines[6].position = position + sf::Vector2f(width, 0.f);
        fadeLines[7].position = position + sf::Vector2f(width, 20.f);

        sf::Vector2f viewCenter = position + sf::Vector2f(width / 2 + drawOffset, 10.f);
        sf::Vector2f viewSize = sf::Vector2f(width, 20.f);
        sf::Vector2f screenSize = oldView.getSize();

        view = sf::View(viewCenter, viewSize);
        view.setViewport({position.x / screenSize.x, position.y / screenSize.y, viewSize.x / screenSize.x, viewSize.y / screenSize.y});
    }

    target.draw(rectangle, states);

    target.setView(view);

    if (startSelection != -1 && startSelection != cursor) {
        target.draw(selectionRect, states);
    }

    target.draw(text, states);

    if (selected && elapsed < sf::seconds(0.5f)) {
        target.draw(cursorLine, states);
    }

    target.setView(oldView);

    target.draw(fadeLines, states);
}

void TextBox::mouseEvent(int x) {
    float mousePos = x + drawOffset;
    cursor = -1;
    for (unsigned int i = 0; i < string.getSize(); ++i) {
        float current = text.findCharacterPos(i).x;
        float next = text.findCharacterPos(i + 1).x;

        if (mousePos <= (next + current) / 2.f) {
            cursor = i;
            break;
        }
    }

    if (cursor == -1) {
        cursor = string.getSize();
    }

    selected = true;
    elapsed = sf::Time::Zero;
    dirty = true;
    updateDrawOffset();
}

void TextBox::removeSelected() {
    if (cursor != startSelection) {
        int min = std::min(cursor, startSelection);
        string.erase(min, std::abs(cursor - startSelection));
        text.setString(string);
        cursor = min;
    }

    startSelection = -1;
    elapsed = sf::Time::Zero;
    dirty = true;
    updateDrawOffset();
}

void TextBox::updateDrawOffset() {
    float cursorPos = text.findCharacterPos(cursor).x - position.x - drawOffset;
    float textSize = text.getGlobalBounds().width;
    if (textSize <= width - 4.f) {
        drawOffset = -2.f;
    } else {
        if (cursorPos < 2.f) {
            drawOffset += cursorPos - 3.f;
        } else if (cursorPos > width - 4.f) {
            drawOffset += cursorPos - width + 4.f;
        }

        if (textSize - drawOffset < width - 4.f) {
            drawOffset = textSize - width + 6.f;
        }
    }
}
