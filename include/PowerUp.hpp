#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <SFML/Graphics.hpp>

enum class PowerUpType {
    EXTRA_LIFE,       // +1 vida
    PADDLE_EXPAND,    // Aumenta tamano de paleta 7s
    PADDLE_SHRINK,    // Reduce tamano de paleta 7s
    PADDLE_FAST,      // Paleta 15% mas rapida 7s
    PADDLE_SLOW,      // Paleta 15% mas lenta 7s
    MULTI_BALL        // Agrega 2 pelotas extra
};

class PowerUp {
private:
    sf::Sprite sprite;
    sf::Vector2f velocity;
    PowerUpType type;
    bool active;
    float fallSpeed;
    
    static sf::Texture textureVidaExtra;
    static sf::Texture textureExpansion;
    static sf::Texture textureContraccion;
    static sf::Texture textureVelocidad;
    static sf::Texture textureLentitud;
    static sf::Texture textureMultiBola;
    static bool texturesLoaded;

public:
    static void LoadTextures();
    PowerUp(float x, float y, PowerUpType type);
    
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);
    void Deactivate();
    
    bool IsActive() const;
    PowerUpType GetType() const;
    sf::FloatRect GetBounds() const;
};

#endif
