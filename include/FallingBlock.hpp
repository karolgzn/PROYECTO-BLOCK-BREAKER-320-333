#ifndef FALLINGBLOCK_HPP
#define FALLINGBLOCK_HPP

#include <SFML/Graphics.hpp>

class FallingBlock {
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    bool active;
    float fallSpeed;

public:
    FallingBlock(float x, float y);
    
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);
    void Deactivate();
    
    bool IsActive() const;
    sf::FloatRect GetBounds() const;
    sf::Vector2f GetPosition() const;
};

#endif
