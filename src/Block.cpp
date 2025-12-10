#include "Block.hpp"

Block::Block(float x, float y, float width, float height, sf::Color color, int points) 
    : destroyed(false), points(points) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
    shape.setPosition(x, y);
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.0f);
}

void Block::Draw(sf::RenderWindow& window) {
    if (!destroyed) {
        window.draw(shape);
    }
}

void Block::Destroy() {
    destroyed = true;
}

bool Block::IsDestroyed() const {
    return destroyed;
}

sf::FloatRect Block::GetBounds() const {
    return shape.getGlobalBounds();
}

int Block::GetPoints() const {
    return points;
}
