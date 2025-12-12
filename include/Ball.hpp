#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float radius;
    float speed;

public:
    Ball(float x, float y, float radius);
    
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);
    void Reset(float x, float y);
    void Launch(float angle);
    void SetVelocity(sf::Vector2f vel);
    void ReverseX();
    void ReverseY();
    void IncreaseSpeed(float percentage);
    void ResetSpeed();
    void SetSpeed(float newSpeed);
    void SetPosition(float x, float y);
    
    sf::Vector2f GetPosition() const;
    sf::Vector2f GetVelocity() const;
    float GetRadius() const;
    sf::FloatRect GetBounds() const;
    float GetSpeed() const;
};

#endif