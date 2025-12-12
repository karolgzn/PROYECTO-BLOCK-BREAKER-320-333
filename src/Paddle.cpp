#include "Paddle.hpp"

Paddle::Paddle(float x, float y, float width, float height) 
    : speed(492.8f), width(width), height(height), slowed(false), slowTimer(0.0f),
      sizeModified(false), speedModified(false), sizeTimer(0.0f), speedTimer(0.0f), useTexture(false) {  // 448 * 1.10 = 492.8
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::Cyan);
    
    sprite.setPosition(x, y);
    
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
    if (useTexture) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

void Paddle::MoveLeft(float deltaTime) {
    shape.move(-speed * deltaTime, 0);
    sprite.setPosition(shape.getPosition());
}

void Paddle::MoveRight(float deltaTime) {
    shape.move(speed * deltaTime, 0);
    sprite.setPosition(shape.getPosition());
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
    // Si hay modificador de velocidad activo, aplicarlo sobre la velocidad reducida
    if (speedModified) {
        speed = slowedSpeed * (modifiedSpeed / normalSpeed);
    } else {
        speed = slowedSpeed;
    }
    shape.setFillColor(slowedColor);
}

void Paddle::UpdateSlow(float deltaTime) {
    if (slowed) {
        slowTimer -= deltaTime;
        if (slowTimer <= 0) {
            slowed = false;
            // Restaurar velocidad considerando si hay modificador activo
            if (speedModified) {
                speed = modifiedSpeed;
            } else {
                speed = normalSpeed;
            }
            shape.setFillColor(normalColor);
        }
    }
}

bool Paddle::IsSlowed() const {
    return slowed;
}

void Paddle::SetSkin(int skinId) {
    std::vector<std::string> skins = {
        "assets/hotdog.png",
        "assets/laboratorio.png",
        "assets/original.png",
        "assets/pan.png",
        "assets/sable.png",
        "assets/trampolin.png"
    };
    
    if (skinId >= 0 && skinId < (int)skins.size()) {
        if (texture.loadFromFile(skins[skinId])) {
            sprite.setTexture(texture);
            float scaleX = width / texture.getSize().x;
            float scaleY = height / texture.getSize().y;
            sprite.setScale(scaleX, scaleY);
            sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
            sprite.setPosition(shape.getPosition());
            useTexture = true;
        }
    }
}

void Paddle::ApplySizeChange(float multiplier, float duration) {
    sizeModified = true;
    sizeTimer = duration;
    width = normalWidth * multiplier;
    shape.setSize(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0f);
    
    // Actualizar sprite si se está usando textura
    if (useTexture && texture.getSize().x > 0) {
        float scaleX = width / texture.getSize().x;
        float scaleY = height / texture.getSize().y;
        sprite.setScale(scaleX, scaleY);
    }
}

void Paddle::ApplySpeedChange(float multiplier, float duration) {
    speedModified = true;
    speedTimer = duration;
    modifiedSpeed = normalSpeed * multiplier;
    // Aplicar el modificador considerando si está en slow
    if (slowed) {
        speed = slowedSpeed * (modifiedSpeed / normalSpeed);
    } else {
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
            
            // Restaurar escala del sprite al tamaño normal
            if (useTexture && texture.getSize().x > 0) {
                float scaleX = width / texture.getSize().x;
                float scaleY = height / texture.getSize().y;
                sprite.setScale(scaleX, scaleY);
            }
        }
    }
    
    // Actualizar efecto de velocidad
    if (speedModified) {
        speedTimer -= deltaTime;
        if (speedTimer <= 0) {
            speedModified = false;
            // Restaurar velocidad considerando si está en slow
            if (slowed) {
                speed = slowedSpeed;
            } else {
                speed = normalSpeed;
            }
        }
    }
}
