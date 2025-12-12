#include "Ball.hpp"
#include <cmath>

Ball::Ball(float x, float y, float radius) 
    : radius(radius), speed(300.0f) {
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::White);
    velocity = sf::Vector2f(0, 0);
}

void Ball::Update(float deltaTime) {
    shape.move(velocity * deltaTime);
}

void Ball::Draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Ball::Reset(float x, float y) {
    shape.setPosition(x, y);
    velocity = sf::Vector2f(0, 0);
}

void Ball::Launch(float angle) {
    // Convertir ángulo a radianes y crear vector de velocidad
    float radians = angle * 3.14159f / 180.0f;
    velocity.x = std::cos(radians) * speed;
    velocity.y = std::sin(radians) * speed;
}

void Ball::SetVelocity(sf::Vector2f vel) {
    velocity = vel;
}

void Ball::ReverseX() {
    velocity.x = -velocity.x;
}

void Ball::ReverseY() {
    velocity.y = -velocity.y;
}

void Ball::SetPosition(float x, float y) {
    shape.setPosition(x, y);
}

sf::Vector2f Ball::GetPosition() const {
    return shape.getPosition();
}

sf::Vector2f Ball::GetVelocity() const {
    return velocity;
}

float Ball::GetRadius() const {
    return radius;
}

sf::FloatRect Ball::GetBounds() const {
    return shape.getGlobalBounds();
}

void Ball::IncreaseSpeed(float percentage) {
    // Aumentar velocidad manteniendo la dirección
    float currentMagnitude = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentMagnitude > 0) {
        sf::Vector2f direction = velocity / currentMagnitude;
        speed *= (1.0f + percentage / 100.0f);
        velocity = direction * speed;
    }
}

void Ball::ResetSpeed() {
    speed = 300.0f;
}

void Ball::SetSpeed(float newSpeed) {
    speed = newSpeed;
    // Actualizar velocidad manteniendo dirección
    float currentMagnitude = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentMagnitude > 0) {
        sf::Vector2f direction = velocity / currentMagnitude;
        velocity = direction * speed;
    }
}

float Ball::GetSpeed() const {
    return speed;
}