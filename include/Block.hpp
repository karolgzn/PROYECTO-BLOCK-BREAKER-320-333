#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <SFML/Graphics.hpp>

class Block {
private:
    sf::RectangleShape shape;
    bool destroyed;
    int points;

public:
    Block(float x, float y, float width, float height, sf::Color color, int points = 10);
    
    void Draw(sf::RenderWindow& window);
    void Destroy();
    
    bool IsDestroyed() const;
    sf::FloatRect GetBounds() const;
    int GetPoints() const;
};

#endif