#include "Paddle.hpp"

Paddle::Paddle(float x, float y, float width, float height) 
    : speed(400.0f), width(width), height(height) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::Cyan);
}

void Paddle::Update(float deltaTime, int windowWidth) {
    // Limitar paddle a los bordes de la ventana
    sf::Vector2f pos = shape.getPosition();
    
    if (pos.x - width / 2.0f < 0) {
        shape.setPosition(width / 2.0f, pos.y);
    }
    if (pos.x + width / 2.0f > windowWidth) {
        shape.setPosition(windowWidth - width / 2.0f, pos.y);
    }
}

void Paddle::Draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Paddle::MoveLeft(float deltaTime) {
    shape.move(-speed * deltaTime, 0);
}

void Paddle::MoveRight(float deltaTime) {
    shape.move(speed * deltaTime, 0);
}

sf::Vector2f Paddle::GetPosition() const {
    return shape.getPosition();
}

sf::FloatRect Paddle::GetBounds() const {
    return shape.getGlobalBounds();
}

float Paddle::GetWidth() const {
    return width;
}