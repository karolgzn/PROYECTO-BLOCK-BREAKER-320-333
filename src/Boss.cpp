#include "Boss.hpp"
#include <cmath>

Boss::Boss(float x, float y) 
    : health(150.0f), maxHealth(150.0f), moveSpeed(150.0f), 
      shootTimer(0.0f), shootCooldown(2.5f), alive(true), movingRight(true) {
    
    // Configurar el cuerpo del jefe
    shape.setSize(sf::Vector2f(100.0f, 50.0f));
    shape.setOrigin(50.0f, 25.0f);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color(180, 0, 0)); // Rojo oscuro
    shape.setOutlineThickness(4.0f);
    shape.setOutlineColor(sf::Color::Black);
    
    velocity = sf::Vector2f(moveSpeed, 0);
    
    // Configurar barra de vida (fondo)
    healthBarBackground.setSize(sf::Vector2f(100.0f, 10.0f));
    healthBarBackground.setFillColor(sf::Color(50, 50, 50));
    healthBarBackground.setOutlineThickness(2.0f);
    healthBarBackground.setOutlineColor(sf::Color::White);
    
    // Configurar barra de vida (frente)
    healthBarForeground.setSize(sf::Vector2f(100.0f, 10.0f));
    healthBarForeground.setFillColor(sf::Color::Green);
}

void Boss::Update(float deltaTime, int windowWidth) {
    if (!alive) return;
    
    // Movimiento horizontal
    shape.move(velocity * deltaTime);
    
    // Rebotar en los bordes
    sf::Vector2f pos = shape.getPosition();
    float halfWidth = 75.0f;
    
    if (pos.x - halfWidth <= 0) {
        shape.setPosition(halfWidth, pos.y);
        velocity.x = moveSpeed;
        movingRight = true;
    }
    else if (pos.x + halfWidth >= windowWidth) {
        shape.setPosition(windowWidth - halfWidth, pos.y);
        velocity.x = -moveSpeed;
        movingRight = false;
    }
    
    // Actualizar timer de disparo
    shootTimer += deltaTime;
    
    // Actualizar posición de la barra de vida
    sf::Vector2f bossPos = shape.getPosition();
    healthBarBackground.setPosition(bossPos.x - 75.0f, bossPos.y - 60.0f);
    healthBarForeground.setPosition(bossPos.x - 75.0f, bossPos.y - 60.0f);
}

void Boss::Draw(sf::RenderWindow& window) {
    if (!alive) return;
    
    window.draw(shape);
    window.draw(healthBarBackground);
    window.draw(healthBarForeground);
}

void Boss::TakeDamage(float damage) {
    if (!alive) return;
    
    health -= damage;
    if (health <= 0) {
        health = 0;
        alive = false;
    }
    
    // Actualizar barra de vida
    float healthPercentage = health / maxHealth;
    healthBarForeground.setSize(sf::Vector2f(150.0f * healthPercentage, 10.0f));
    
    // Cambiar color según vida restante
    if (healthPercentage > 0.6f) {
        healthBarForeground.setFillColor(sf::Color::Green);
    }
    else if (healthPercentage > 0.3f) {
        healthBarForeground.setFillColor(sf::Color::Yellow);
    }
    else {
        healthBarForeground.setFillColor(sf::Color::Red);
    }
}

bool Boss::ShouldShoot() {
    if (!alive) return false;
    return shootTimer >= shootCooldown;
}

void Boss::ResetShootTimer() {
    shootTimer = 0.0f;
}

sf::Vector2f Boss::GetShootPosition() const {
    sf::Vector2f pos = shape.getPosition();
    return sf::Vector2f(pos.x, pos.y + 40.0f); // Dispara desde abajo
}

sf::FloatRect Boss::GetBounds() const {
    return shape.getGlobalBounds();
}

bool Boss::IsAlive() const {
    return alive;
}

float Boss::GetHealth() const {
    return health;
}

sf::Vector2f Boss::GetPosition() const {
    return shape.getPosition();
}
