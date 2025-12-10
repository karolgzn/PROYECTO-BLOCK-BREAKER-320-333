#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Block.hpp"

// Constantes
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float BALL_RADIUS = 8.0f;
const float PADDLE_WIDTH = 100.0f;
const float PADDLE_HEIGHT = 15.0f;
const float BLOCK_WIDTH = 75.0f;
const float BLOCK_HEIGHT = 25.0f;
const int BLOCKS_PER_ROW = 10;
const int BLOCK_ROWS = 5;

// Función para verificar colisión círculo-rectángulo
bool CheckCollision(const Ball& ball, const sf::FloatRect& rect, bool& hitTop) {
    sf::Vector2f ballPos = ball.GetPosition();
    float radius = ball.GetRadius();
    
    // Encontrar el punto más cercano del rectángulo al centro de la pelota
    float closestX = std::max(rect.left, std::min(ballPos.x, rect.left + rect.width));
    float closestY = std::max(rect.top, std::min(ballPos.y, rect.top + rect.height));
    
    // Calcular distancia entre el punto más cercano y el centro de la pelota
    float distanceX = ballPos.x - closestX;
    float distanceY = ballPos.y - closestY;
    float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    
    // Verificar si hay colisión
    if (distanceSquared < (radius * radius)) {
        // Determinar si golpeó arriba/abajo o los lados
        hitTop = std::abs(distanceY) > std::abs(distanceX);
        return true;
    }
    
    return false;
}

int main() {
    // Crear ventana
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arkanoid - SFML");
    
    if (!window.isOpen()) {
        std::cerr << "Error: No se pudo crear la ventana SFML" << std::endl;
        return -1;
    }
    
    window.setFramerateLimit(60);
    std::cout << "Juego iniciado - Presiona ESPACIO para lanzar la pelota" << std::endl;
    
    // Variables del juego
    int score = 0;
    int lives = 3;
    bool gameStarted = false;
    bool gameOver = false;
    bool victory = false;
    
    // Crear objetos del juego
    Ball ball(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, BALL_RADIUS);
    Paddle paddle(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 50.0f, PADDLE_WIDTH, PADDLE_HEIGHT);
    
    // Crear bloques
    std::vector<Block> blocks;
    sf::Color colors[] = {
        sf::Color::Red, 
        sf::Color::Yellow, 
        sf::Color::Green, 
        sf::Color::Blue, 
        sf::Color::Magenta
    };
    
    float startX = (WINDOW_WIDTH - (BLOCKS_PER_ROW * BLOCK_WIDTH)) / 2.0f + BLOCK_WIDTH / 2.0f;
    float startY = 80.0f;
    
    for (int row = 0; row < BLOCK_ROWS; ++row) {
        for (int col = 0; col < BLOCKS_PER_ROW; ++col) {
            float x = startX + col * BLOCK_WIDTH;
            float y = startY + row * (BLOCK_HEIGHT + 5.0f);
            blocks.emplace_back(x, y, BLOCK_WIDTH - 5.0f, BLOCK_HEIGHT, colors[row], 10);
        }
    }
    
    // Configurar texto (opcional, funciona sin fuente)
    sf::Font font;
    bool fontLoaded = font.loadFromFile("assets/fonts/arial.ttf");
    
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text messageText;
    
    if (fontLoaded) {
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        
        livesText.setFont(font);
        livesText.setCharacterSize(20);
        livesText.setFillColor(sf::Color::White);
        livesText.setPosition(WINDOW_WIDTH - 120, 10);
        
        messageText.setFont(font);
        messageText.setCharacterSize(24);
        messageText.setFillColor(sf::Color::White);
    }
    
    // Reloj para delta time
    sf::Clock clock;
    
    // Game Loop
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Eventos
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && !gameStarted && !gameOver) {
                    gameStarted = true;
                    ball.Launch(-60.0f); // Lanzar hacia arriba-izquierda
                }
                
                if (event.key.code == sf::Keyboard::R && gameOver) {
                    // Reiniciar juego
                    score = 0;
                    lives = 3;
                    gameStarted = false;
                    gameOver = false;
                    victory = false;
                    
                    ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                    
                    // Recrear bloques
                    blocks.clear();
                    for (int row = 0; row < BLOCK_ROWS; ++row) {
                        for (int col = 0; col < BLOCKS_PER_ROW; ++col) {
                            float x = startX + col * BLOCK_WIDTH;
                            float y = startY + row * (BLOCK_HEIGHT + 5.0f);
                            blocks.emplace_back(x, y, BLOCK_WIDTH - 5.0f, BLOCK_HEIGHT, colors[row], 10);
                        }
                    }
                }
            }
        }
        
        if (!gameOver && gameStarted) {
            // Control del paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || 
                sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                paddle.MoveLeft(deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || 
                sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                paddle.MoveRight(deltaTime);
            }
            
            // Actualizar objetos
            ball.Update(deltaTime);
            paddle.Update(deltaTime, WINDOW_WIDTH);
            
            // Colisiones con paredes
            sf::Vector2f ballPos = ball.GetPosition();
            
            // Pared izquierda
            if (ballPos.x - BALL_RADIUS < 0) {
                ball.ReverseX();
            }
            // Pared derecha
            if (ballPos.x + BALL_RADIUS > WINDOW_WIDTH) {
                ball.ReverseX();
            }
            // Pared superior
            if (ballPos.y - BALL_RADIUS < 0) {
                ball.ReverseY();
            }
            
            // Pelota cayó
            if (ballPos.y - BALL_RADIUS > WINDOW_HEIGHT) {
                lives--;
                std::cout << "Vida perdida! Vidas restantes: " << lives << std::endl;
                
                if (lives <= 0) {
                    gameOver = true;
                    victory = false;
                    std::cout << "Game Over! Puntuacion final: " << score << std::endl;
                } else {
                    ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                    gameStarted = false;
                }
            }
            
            // Colisión con paddle
            bool hitTop;
            if (CheckCollision(ball, paddle.GetBounds(), hitTop)) {
                ball.ReverseY();
                
                // Modificar ángulo basado en dónde golpeó el paddle
                sf::Vector2f paddlePos = paddle.GetPosition();
                float relativeIntersect = (ballPos.x - paddlePos.x) / (paddle.GetWidth() / 2.0f);
                float angle = relativeIntersect * 60.0f; // Máximo 60 grados
                
                sf::Vector2f velocity = ball.GetVelocity();
                float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                
                float radians = (angle - 90.0f) * 3.14159f / 180.0f;
                ball.SetVelocity(sf::Vector2f(std::cos(radians) * speed, std::sin(radians) * speed));
            }
            
            // Colisión con bloques
            for (auto& block : blocks) {
                if (!block.IsDestroyed()) {
                    if (CheckCollision(ball, block.GetBounds(), hitTop)) {
                        block.Destroy();
                        score += block.GetPoints();
                        std::cout << "Bloque destruido! Puntos: " << score << std::endl;
                        
                        if (hitTop) {
                            ball.ReverseY();
                        } else {
                            ball.ReverseX();
                        }
                        break;
                    }
                }
            }
            
            // Verificar victoria
            int blocksRemaining = 0;
            for (const auto& block : blocks) {
                if (!block.IsDestroyed()) {
                    blocksRemaining++;
                }
            }
            
            if (blocksRemaining == 0) {
                gameOver = true;
                victory = true;
                std::cout << "Victoria! Puntuacion final: " << score << std::endl;
            }
        }
        
        // Si no ha empezado, la pelota sigue al paddle
        if (!gameStarted && !gameOver) {
            sf::Vector2f paddlePos = paddle.GetPosition();
            ball.Reset(paddlePos.x, paddlePos.y - 30.0f);
        }
        
        // Actualizar textos
        if (fontLoaded) {
            scoreText.setString("Puntos: " + std::to_string(score));
            livesText.setString("Vidas: " + std::to_string(lives));
        }
        
        // Renderizado
        window.clear(sf::Color(20, 20, 40));
        
        // Dibujar objetos
        paddle.Draw(window);
        ball.Draw(window);
        
        for (auto& block : blocks) {
            block.Draw(window);
        }
        
        // Dibujar UI
        if (fontLoaded) {
            window.draw(scoreText);
            window.draw(livesText);
            
            if (!gameStarted && !gameOver) {
                messageText.setString("Presiona ESPACIO para iniciar");
                messageText.setPosition(WINDOW_WIDTH / 2.0f - 180, WINDOW_HEIGHT / 2.0f);
                window.draw(messageText);
            }
            
            if (gameOver) {
                if (victory) {
                    messageText.setString("VICTORIA!");
                    messageText.setFillColor(sf::Color::Green);
                } else {
                    messageText.setString("GAME OVER");
                    messageText.setFillColor(sf::Color::Red);
                }
                messageText.setPosition(WINDOW_WIDTH / 2.0f - 80, WINDOW_HEIGHT / 2.0f - 50);
                window.draw(messageText);
                
                messageText.setString("Presiona R para reiniciar");
                messageText.setFillColor(sf::Color::White);
                messageText.setCharacterSize(20);
                messageText.setPosition(WINDOW_WIDTH / 2.0f - 140, WINDOW_HEIGHT / 2.0f + 20);
                window.draw(messageText);
            }
        }
        
        window.display();
    }
    
    return 0;
}
