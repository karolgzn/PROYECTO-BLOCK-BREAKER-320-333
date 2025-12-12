#include "Paddle.hpp"

Paddle::Paddle(float x, float y, float width, float height) 
    : speed(492.8f), width(width), height(height), slowed(false), slowTimer(0.0f),
      sizeModified(false), speedModified(false), sizeTimer(0.0f), speedTimer(0.0f) {  // 448 * 1.10 = 492.8
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::Cyan);
    
    normalSpeed = speed;
    normalWidth = width;
    normalColor = sf::Color::Cyan;
    slowedColor = sf::Color(0, 100, 150); // Azul oscuro
}

void Paddle::Update(float deltaTime, int windowWidth) {
    // Actualizar slowdown
    UpdateSlow(deltaTime);
    
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

void Paddle::ApplySlow(float duration, float percentage) {
    slowed = true;
    slowTimer = duration;
    slowedSpeed = normalSpeed * (1.0f - percentage); // 60% slower = 40% de velocidad
    speed = slowedSpeed;
    shape.setFillColor(slowedColor);
}

void Paddle::UpdateSlow(float deltaTime) {
    if (slowed) {
        slowTimer -= deltaTime;
        if (slowTimer <= 0) {
            slowed = false;
            speed = normalSpeed;
            shape.setFillColor(normalColor);
        }
    }
}

bool Paddle::IsSlowed() const {
    return slowed;
}

void Paddle::ApplySizeChange(float multiplier, float duration) {
    sizeModified = true;
    sizeTimer = duration;
    width = normalWidth * multiplier;
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
}

void Paddle::ApplySpeedChange(float multiplier, float duration) {
    speedModified = true;
    speedTimer = duration;
    modifiedSpeed = normalSpeed * multiplier;
    if (!slowed) {
        speed = modifiedSpeed;
    }
}

void Paddle::UpdatePowerUps(float deltaTime) {
    // Actualizar efecto de tamano
    if (sizeModified) {
        sizeTimer -= deltaTime;
        if (sizeTimer <= 0) {
            sizeModified = false;
            width = normalWidth;
            shape.setSize(sf::Vector2f(width, height));
            shape.setOrigin(width / 2.0f, height / 2.0f);
        }
    }
    
    // Actualizar efecto de velocidad
    if (speedModified) {
        speedTimer -= deltaTime;
        if (speedTimer <= 0) {
            speedModified = false;
            if (!slowed) {
                speed = normalSpeed;
            }
        }
    }
}
