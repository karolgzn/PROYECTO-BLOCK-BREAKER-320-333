#include "Block.hpp"

Block::Block(float x, float y, float width, float height, sf::Color color, int points) 
    : destroyed(false), points(points) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
    shape.setPosition(x, y);
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color(0, 0, 0, 150));
    shape.setOutlineThickness(1.0f);
    
    // Crear efecto de brillo superior/izquierdo (retro)
    float borderSize = 3.0f;
    highlight.setSize(sf::Vector2f(width - borderSize * 2, height - borderSize * 2));
    highlight.setOrigin((width - borderSize * 2) / 2.0f, (height - borderSize * 2) / 2.0f);
    highlight.setPosition(x - 1.0f, y - 1.0f);
    sf::Color lightColor = color;
    lightColor.r = std::min(255, (int)color.r + 60);
    lightColor.g = std::min(255, (int)color.g + 60);
    lightColor.b = std::min(255, (int)color.b + 60);
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineColor(lightColor);
    highlight.setOutlineThickness(borderSize);
    
    // Crear efecto de sombra inferior/derecha (retro)
    shadow.setSize(sf::Vector2f(width - borderSize * 2, height - borderSize * 2));
    shadow.setOrigin((width - borderSize * 2) / 2.0f, (height - borderSize * 2) / 2.0f);
    shadow.setPosition(x + 1.0f, y + 1.0f);
    sf::Color darkColor = color;
    darkColor.r = std::max(0, (int)color.r - 100);
    darkColor.g = std::max(0, (int)color.g - 100);
    darkColor.b = std::max(0, (int)color.b - 100);
    shadow.setFillColor(sf::Color::Transparent);
    shadow.setOutlineColor(darkColor);
    shadow.setOutlineThickness(borderSize);
}

void Block::Draw(sf::RenderWindow& window) {
    if (!destroyed) {
        window.draw(shape);
        window.draw(shadow);
        window.draw(highlight);
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