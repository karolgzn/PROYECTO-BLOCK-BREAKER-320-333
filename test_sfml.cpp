#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    std::cout << "Creando ventana de prueba SFML..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode(400, 300), "Test SFML");
    
    if (!window.isOpen()) {
        std::cerr << "ERROR: No se pudo crear la ventana!" << std::endl;
        return -1;
    }
    
    std::cout << "Ventana creada exitosamente!" << std::endl;
    std::cout << "Presiona ESC para cerrar" << std::endl;
    
    sf::CircleShape circle(50);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(175, 125);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }
        
        window.clear(sf::Color::Black);
        window.draw(circle);
        window.display();
    }
    
    std::cout << "Ventana cerrada correctamente" << std::endl;
    return 0;
}
