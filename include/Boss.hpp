#ifndef BOSS_HPP
#define BOSS_HPP

#include <SFML/Graphics.hpp>

class Boss {
private:
    sf::RectangleShape shape;
    sf::RectangleShape healthBarBackground;
    sf::RectangleShape healthBarForeground;
    float health;
    float maxHealth;
    sf::Vector2f velocity;
    float moveSpeed;
    float shootTimer;
    float shootCooldown;
    bool alive;
    bool movingRight;

public:
    Boss(float x, float y);
    
    void Update(float deltaTime, int windowWidth);
    void Draw(sf::RenderWindow& window);
    void TakeDamage(float damage);
    bool ShouldShoot();
    void ResetShootTimer();
    
    sf::Vector2f GetShootPosition() const;
    sf::FloatRect GetBounds() const;
    bool IsAlive() const;
    float GetHealth() const;
    sf::Vector2f GetPosition() const;
};

#endif
