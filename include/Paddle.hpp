#ifndef PADDLE_HPP
#define PADDLE_HPP

#include <SFML/Graphics.hpp>

class Paddle {
private:
    sf::RectangleShape shape;
    sf::Texture texture;
    sf::Sprite sprite;
    bool useTexture;
    float speed;
    float width;
    float height;
    bool slowed;
    float slowTimer;
    float normalSpeed;
    float slowedSpeed;
    sf::Color normalColor;
    sf::Color slowedColor;
    
    // PowerUp effects
    bool sizeModified;
    bool speedModified;
    float sizeTimer;
    float normalWidth;
    float modifiedSpeed;
    float speedTimer;

public:
    Paddle(float x, float y, float width, float height);
    
    void Update(float deltaTime, int windowWidth);
    void Draw(sf::RenderWindow& window);
    void MoveLeft(float deltaTime);
    void MoveRight(float deltaTime);
    void ApplySlow(float duration, float percentage);
    void UpdateSlow(float deltaTime);
    void ApplySizeChange(float multiplier, float duration);
    void ApplySpeedChange(float multiplier, float duration);
    void UpdatePowerUps(float deltaTime);
    void SetSkin(int skinId);
    
    sf::Vector2f GetPosition() const;
    sf::FloatRect GetBounds() const;
    float GetWidth() const;
    bool IsSlowed() const;
};

#endif