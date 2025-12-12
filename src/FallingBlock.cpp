#include "FallingBlock.hpp"

FallingBlock::FallingBlock(float x, float y) 
    : active(true), fallSpeed(200.0f) {
    
    shape.setSize(sf::Vector2f(25.0f, 25.0f));
    shape.setOrigin(12.5f, 12.5f);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color(255, 140, 0)); // Naranja oscuro
    shape.setOutlineThickness(3.0f);
    shape.setOutlineColor(sf::Color(255, 69, 0)); // Naranja-rojo brillante
    
    velocity = sf::Vector2f(0, fallSpeed);
}

void FallingBlock::Update(float deltaTime) {
    if (!active) return;
    
    shape.move(velocity * deltaTime);
    
    // Desactivar si sale de la pantalla (cayó al suelo)
    if (shape.getPosition().y > 620.0f) { // Más allá del fondo de la ventana
        active = false;
    }
}

void FallingBlock::Draw(sf::RenderWindow& window) {
    if (active) {
        window.draw(shape);
    }
}

void FallingBlock::Deactivate() {
    active = false;
}

bool FallingBlock::IsActive() const {
    return active;
}

sf::FloatRect FallingBlock::GetBounds() const {
    return shape.getGlobalBounds();
}

sf::Vector2f FallingBlock::GetPosition() const {
    return shape.getPosition();
}
