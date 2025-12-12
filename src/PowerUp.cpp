#include "PowerUp.hpp"
#include <iostream>

// Definir texturas estaticas
sf::Texture PowerUp::textureVidaExtra;
sf::Texture PowerUp::textureExpansion;
sf::Texture PowerUp::textureContraccion;
sf::Texture PowerUp::textureVelocidad;
sf::Texture PowerUp::textureLentitud;
sf::Texture PowerUp::textureMultiBola;
bool PowerUp::texturesLoaded = false;

void PowerUp::LoadTextures() {
    if (!texturesLoaded) {
        if (!textureVidaExtra.loadFromFile("assets/vida_extra.png")) {
            std::cerr << "Error cargando vida_extra.png" << std::endl;
            sf::Image img; img.create(30, 30, sf::Color::Magenta);
            textureVidaExtra.loadFromImage(img);
        }
        if (!textureExpansion.loadFromFile("assets/expansion.png")) {
            std::cerr << "Error cargando expansion.png" << std::endl;
            sf::Image img; img.create(30, 30, sf::Color::Green);
            textureExpansion.loadFromImage(img);
        }
        if (!textureContraccion.loadFromFile("assets/contraccion.png")) {
            std::cerr << "Error cargando contraccion.png" << std::endl;
            sf::Image img; img.create(30, 30, sf::Color::Red);
            textureContraccion.loadFromImage(img);
        }
        if (!textureVelocidad.loadFromFile("assets/velocidad.png")) {
            std::cerr << "Error cargando velocidad.png" << std::endl;
            sf::Image img; img.create(30, 30, sf::Color::Cyan);
            textureVelocidad.loadFromImage(img);
        }
        if (!textureLentitud.loadFromFile("assets/lentitud.png")) {
            std::cerr << "Error cargando lentitud.png" << std::endl;
            sf::Image img; img.create(30, 30, sf::Color(139, 69, 19));
            textureLentitud.loadFromImage(img);
        }
        if (!textureMultiBola.loadFromFile("assets/multi_bola.png")) {
            std::cerr << "Error cargando multi_bola.png" << std::endl;
            sf::Image img; img.create(30, 30, sf::Color::Yellow);
            textureMultiBola.loadFromImage(img);
        }
        texturesLoaded = true;
    }
}

PowerUp::PowerUp(float x, float y, PowerUpType type) 
    : type(type), active(true), fallSpeed(150.0f) {
    
    // Asignar textura segun tipo
    switch(type) {
        case PowerUpType::EXTRA_LIFE:
            sprite.setTexture(textureVidaExtra);
            break;
        case PowerUpType::PADDLE_EXPAND:
            sprite.setTexture(textureExpansion);
            break;
        case PowerUpType::PADDLE_SHRINK:
            sprite.setTexture(textureContraccion);
            break;
        case PowerUpType::PADDLE_FAST:
            sprite.setTexture(textureVelocidad);
            break;
        case PowerUpType::PADDLE_SLOW:
            sprite.setTexture(textureLentitud);
            break;
        case PowerUpType::MULTI_BALL:
            sprite.setTexture(textureMultiBola);
            break;
    }
    
    const sf::Texture* currentTexture = sprite.getTexture();
    if (currentTexture) {
        // Escalar a 30x30 pixels
        float scaleX = 30.0f / currentTexture->getSize().x;
        float scaleY = 30.0f / currentTexture->getSize().y;
        sprite.setScale(scaleX, scaleY);
        sprite.setOrigin(currentTexture->getSize().x / 2.0f, currentTexture->getSize().y / 2.0f);
    }
    sprite.setPosition(x, y);
    
    velocity = sf::Vector2f(0, fallSpeed);
}

void PowerUp::Update(float deltaTime) {
    if (!active) return;
    
    sprite.move(velocity * deltaTime);
    
    // Desactivar si sale de la pantalla
    if (sprite.getPosition().y > 620.0f) {
        active = false;
    }
}

void PowerUp::Draw(sf::RenderWindow& window) {
    if (active) {
        window.draw(sprite);
    }
}

void PowerUp::Deactivate() {
    active = false;
}

bool PowerUp::IsActive() const {
    return active;
}

PowerUpType PowerUp::GetType() const {
    return type;
}

sf::FloatRect PowerUp::GetBounds() const {
    return sprite.getGlobalBounds();
}
