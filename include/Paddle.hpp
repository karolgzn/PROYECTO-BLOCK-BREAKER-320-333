#ifndef PADDLE_HPP
#define PADDLE_HPP

#include <SFML/Graphics.hpp>

class Paddle {
private:
    sf::RectangleShape shape;
    float speed;
    float width;
    float height;

public:
    Paddle(float x, float y, float width, float height);
    
    void Update(float deltaTime, int windowWidth);
    void Draw(sf::RenderWindow& window);
    void MoveLeft(float deltaTime);
    void MoveRight(float deltaTime);
    
    sf::Vector2f GetPosition() const;
    sf::FloatRect GetBounds() const;
    float GetWidth() const;
};

#endif