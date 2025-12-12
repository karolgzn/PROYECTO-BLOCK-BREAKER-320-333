#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Block.hpp"

// Estados del juego
enum GameState {
    MENU,
    PLAYING,
    CONTROLS,
    GAME_OVER
};

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

// Función para crear un botón visual
void CreateButton(sf::RectangleShape& button, sf::Text& text, const std::string& label, 
                  float x, float y, float width, float height, const sf::Font& font) {
    button.setSize(sf::Vector2f(width, height));
    button.setPosition(x, y);
    button.setFillColor(sf::Color(70, 70, 100));
    button.setOutlineThickness(3);
    button.setOutlineColor(sf::Color::White);
    
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, 
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

// Función para verificar si el mouse está sobre un botón
bool IsMouseOver(const sf::RectangleShape& button, const sf::Vector2i& mousePos) {
    return button.getGlobalBounds().contains(static_cast<float>(mousePos.x), 
                                            static_cast<float>(mousePos.y));
}

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
    std::cout << "Juego iniciado - Usa el menu principal" << std::endl;
    
    // Variables del juego
    GameState gameState = MENU;
    int score = 0;
    int lives = 3;
    bool gameStarted = false;
    bool gameOver = false;
    bool victory = false;
    int lastSpeedIncreaseScore = 0;
    const float MAX_BALL_SPEED = 600.0f; // 100% más rápido (2x velocidad inicial)
    
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
    
    // Configurar texto con múltiples opciones de fuente
    sf::Font font;
    bool fontLoaded = false;
    
    // Intentar cargar fuente del proyecto primero
    if (font.loadFromFile("assets/fonts/arial.ttf")) {
        fontLoaded = true;
        std::cout << "Fuente cargada desde assets/fonts/arial.ttf" << std::endl;
    }
    // Si no existe, intentar fuente del sistema Windows
    else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
        std::cout << "Fuente cargada desde Windows/Fonts" << std::endl;
    }
    else {
        std::cout << "Advertencia: No se pudo cargar fuente. El texto no se mostrará." << std::endl;
    }
    
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
    
    // Elementos del menú
    sf::Text titleText;
    sf::RectangleShape playButton, controlsButton, exitButton;
    sf::Text playText, controlsText, exitText;
    sf::RectangleShape backButton;
    sf::Text backText;
    
    // Botones de Game Over
    sf::RectangleShape gameOverMenuButton, gameOverRetryButton, gameOverExitButton;
    sf::Text gameOverMenuText, gameOverRetryText, gameOverExitText;
    
    if (fontLoaded) {
        // Título del juego
        titleText.setFont(font);
        titleText.setString("ARKANOID");
        titleText.setCharacterSize(72);
        titleText.setFillColor(sf::Color::Cyan);
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f, 
                           titleBounds.top + titleBounds.height / 2.0f);
        titleText.setPosition(WINDOW_WIDTH / 2.0f, 120);
        
        // Crear botones del menú
        CreateButton(playButton, playText, "INICIAR JUEGO", 
                    WINDOW_WIDTH / 2.0f - 150, 250, 300, 60, font);
        CreateButton(controlsButton, controlsText, "CONTROLES", 
                    WINDOW_WIDTH / 2.0f - 150, 330, 300, 60, font);
        CreateButton(exitButton, exitText, "SALIR", 
                    WINDOW_WIDTH / 2.0f - 150, 410, 300, 60, font);
        
        // Botón de regresar (para pantalla de controles)
        CreateButton(backButton, backText, "REGRESAR", 
                    WINDOW_WIDTH / 2.0f - 100, 500, 200, 50, font);
        
        // Botones de Game Over
        CreateButton(gameOverMenuButton, gameOverMenuText, "MENU", 
                    WINDOW_WIDTH / 2.0f - 350, 350, 200, 60, font);
        CreateButton(gameOverRetryButton, gameOverRetryText, "REINTENTAR", 
                    WINDOW_WIDTH / 2.0f - 100, 350, 200, 60, font);
        CreateButton(gameOverExitButton, gameOverExitText, "SALIR", 
                    WINDOW_WIDTH / 2.0f + 150, 350, 200, 60, font);
    }
    
    // Configurar música de fondo
    sf::Music backgroundMusic;
    bool musicLoaded = false;
    
    if (backgroundMusic.openFromFile("assets/music/background_music.ogg")) {
        musicLoaded = true;
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.0f); // 50% volumen
        backgroundMusic.play();
        std::cout << "Música de fondo cargada y reproduciendo" << std::endl;
    }
    else if (backgroundMusic.openFromFile("assets/music/background_music.wav")) {
        musicLoaded = true;
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.0f);
        backgroundMusic.play();
        std::cout << "Música de fondo cargada y reproduciendo" << std::endl;
    }
    else {
        std::cout << "Advertencia: No se encontró música de fondo. El juego seguirá sin música." << std::endl;
        std::cout << "Coloca un archivo 'background_music.ogg' en assets/music/" << std::endl;
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
            
            // Manejo de clics del mouse
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                
                if (gameState == MENU) {
                    if (IsMouseOver(playButton, mousePos)) {
                        gameState = PLAYING;
                        std::cout << "Juego iniciado desde el menú" << std::endl;
                    }
                    else if (IsMouseOver(controlsButton, mousePos)) {
                        gameState = CONTROLS;
                        std::cout << "Mostrando controles" << std::endl;
                    }
                    else if (IsMouseOver(exitButton, mousePos)) {
                        window.close();
                    }
                }
                else if (gameState == CONTROLS) {
                    if (IsMouseOver(backButton, mousePos)) {
                        gameState = MENU;
                    }
                }
                else if (gameState == PLAYING && gameOver) {
                    // Manejo de clics en pantalla de Game Over
                    if (IsMouseOver(gameOverMenuButton, mousePos)) {
                        // Volver al menú
                        gameState = MENU;
                        gameStarted = false;
                        gameOver = false;
                        victory = false;
                        score = 0;
                        lives = 3;
                        lastSpeedIncreaseScore = 0;
                        ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                        ball.ResetSpeed();
                        
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
                    else if (IsMouseOver(gameOverRetryButton, mousePos)) {
                        // Reintentar
                        gameStarted = false;
                        gameOver = false;
                        victory = false;
                        score = 0;
                        lives = 3;
                        lastSpeedIncreaseScore = 0;
                        ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                        ball.ResetSpeed();
                        
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
                    else if (IsMouseOver(gameOverExitButton, mousePos)) {
                        // Salir del juego
                        window.close();
                    }
                }
            }
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && gameState == PLAYING && !gameStarted && !gameOver) {
                    gameStarted = true;
                    ball.Launch(-60.0f); // Lanzar hacia arriba-izquierda
                }
                
                // Control de música con tecla M
                if (event.key.code == sf::Keyboard::M && musicLoaded) {
                    if (backgroundMusic.getStatus() == sf::Music::Playing) {
                        backgroundMusic.pause();
                        std::cout << "Música pausada" << std::endl;
                    } else {
                        backgroundMusic.play();
                        std::cout << "Música reanudada" << std::endl;
                    }
                }
                
                // Regresar al menú desde Game Over
                if (event.key.code == sf::Keyboard::Escape && gameState == PLAYING) {
                    gameState = MENU;
                    gameStarted = false;
                    gameOver = false;
                }
                
                if (event.key.code == sf::Keyboard::R && gameOver) {
                    // Reiniciar juego
                    gameState = PLAYING;
                    score = 0;
                    lives = 3;
                    lastSpeedIncreaseScore = 0;
                    gameStarted = false;
                    gameOver = false;
                    victory = false;
                    
                    ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                    ball.ResetSpeed();
                    
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
        
        if (gameState == PLAYING && !gameOver && gameStarted) {
            // Control del paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                paddle.MoveLeft(deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
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
                        
                        // Aumentar velocidad cada 70 puntos
                        if (score - lastSpeedIncreaseScore >= 70) {
                            if (ball.GetSpeed() < MAX_BALL_SPEED) {
                                ball.IncreaseSpeed(5.0f); // 5% más rápido
                                lastSpeedIncreaseScore = score;
                                std::cout << "¡Velocidad de bola aumentada! Velocidad actual: " 
                                         << ball.GetSpeed() << std::endl;
                            }
                        }
                        
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
        if (gameState == PLAYING && !gameStarted && !gameOver) {
            sf::Vector2f paddlePos = paddle.GetPosition();
            ball.Reset(paddlePos.x, paddlePos.y - 30.0f);
        }
        
        // Actualizar textos
        if (fontLoaded) {
            scoreText.setString("Puntuacion: " + std::to_string(score));
            livesText.setString("Vidas: " + std::to_string(lives));
        }
        
        // Renderizado
        window.clear(sf::Color(20, 20, 40));
        
        if (gameState == MENU) {
            // Dibujar menú principal
            if (fontLoaded) {
                window.draw(titleText);
                
                // Efecto hover en botones
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                
                if (IsMouseOver(playButton, mousePos)) {
                    playButton.setFillColor(sf::Color(100, 100, 150));
                } else {
                    playButton.setFillColor(sf::Color(70, 70, 100));
                }
                
                if (IsMouseOver(controlsButton, mousePos)) {
                    controlsButton.setFillColor(sf::Color(100, 100, 150));
                } else {
                    controlsButton.setFillColor(sf::Color(70, 70, 100));
                }
                
                if (IsMouseOver(exitButton, mousePos)) {
                    exitButton.setFillColor(sf::Color(100, 100, 150));
                } else {
                    exitButton.setFillColor(sf::Color(70, 70, 100));
                }
                
                window.draw(playButton);
                window.draw(playText);
                window.draw(controlsButton);
                window.draw(controlsText);
                window.draw(exitButton);
                window.draw(exitText);
            }
        }
        else if (gameState == CONTROLS) {
            // Dibujar pantalla de controles
            if (fontLoaded) {
                sf::Text controlsTitle;
                controlsTitle.setFont(font);
                controlsTitle.setString("CONTROLES");
                controlsTitle.setCharacterSize(48);
                controlsTitle.setFillColor(sf::Color::Cyan);
                sf::FloatRect ctBounds = controlsTitle.getLocalBounds();
                controlsTitle.setOrigin(ctBounds.left + ctBounds.width / 2.0f, 
                                       ctBounds.top + ctBounds.height / 2.0f);
                controlsTitle.setPosition(WINDOW_WIDTH / 2.0f, 80);
                window.draw(controlsTitle);
                
                sf::Text controlsList;
                controlsList.setFont(font);
                controlsList.setCharacterSize(24);
                controlsList.setFillColor(sf::Color::White);
                controlsList.setString(
                    "FLECHA IZQUIERDA: Mover paleta a la izquierda\n\n"
                    "FLECHA DERECHA: Mover paleta a la derecha\n\n"
                    "ESPACIO: Lanzar pelota\n\n"
                    "M: Pausar/Reanudar musica\n\n"
                    "R: Reiniciar juego (en Game Over)\n\n"
                    "ESC: Volver al menu"
                );
                controlsList.setPosition(150, 180);
                window.draw(controlsList);
                
                // Botón de regresar con hover
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (IsMouseOver(backButton, mousePos)) {
                    backButton.setFillColor(sf::Color(100, 100, 150));
                } else {
                    backButton.setFillColor(sf::Color(70, 70, 100));
                }
                
                window.draw(backButton);
                window.draw(backText);
            }
        }
        else if (gameState == PLAYING) {
            // Dibujar objetos del juego
            paddle.Draw(window);
            ball.Draw(window);
            
            for (auto& block : blocks) {
                block.Draw(window);
            }
            
            // Dibujar UI del juego
            if (fontLoaded) {
                window.draw(scoreText);
                window.draw(livesText);
                
                if (!gameStarted && !gameOver) {
                    messageText.setString("Presiona ESPACIO para iniciar");
                    messageText.setCharacterSize(24);
                    messageText.setFillColor(sf::Color::White);
                    sf::FloatRect msgBounds = messageText.getLocalBounds();
                    messageText.setOrigin(msgBounds.left + msgBounds.width / 2.0f, 
                                         msgBounds.top + msgBounds.height / 2.0f);
                    messageText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                    window.draw(messageText);
                }
                
                if (gameOver) {
                    // Overlay oscuro semi-transparente
                    sf::RectangleShape overlay;
                    overlay.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                    overlay.setPosition(0, 0);
                    overlay.setFillColor(sf::Color(0, 0, 0, 200)); // Negro con 200/255 de opacidad
                    window.draw(overlay);
                    
                    // Título de Game Over o Victoria
                    if (victory) {
                        messageText.setString("¡VICTORIA!");
                        messageText.setFillColor(sf::Color::Green);
                    } else {
                        messageText.setString("GAME OVER");
                        messageText.setFillColor(sf::Color::Red);
                    }
                    messageText.setCharacterSize(64);
                    messageText.setStyle(sf::Text::Bold);
                    sf::FloatRect msgBounds = messageText.getLocalBounds();
                    messageText.setOrigin(msgBounds.left + msgBounds.width / 2.0f, 
                                         msgBounds.top + msgBounds.height / 2.0f);
                    messageText.setPosition(WINDOW_WIDTH / 2.0f, 150);
                    window.draw(messageText);
                    
                    // Mostrar puntuación final
                    messageText.setString("Puntuacion Final: " + std::to_string(score));
                    messageText.setFillColor(sf::Color::White);
                    messageText.setCharacterSize(32);
                    messageText.setStyle(sf::Text::Regular);
                    msgBounds = messageText.getLocalBounds();
                    messageText.setOrigin(msgBounds.left + msgBounds.width / 2.0f, 
                                         msgBounds.top + msgBounds.height / 2.0f);
                    messageText.setPosition(WINDOW_WIDTH / 2.0f, 240);
                    window.draw(messageText);
                    
                    // Botones con efecto hover
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    
                    if (IsMouseOver(gameOverMenuButton, mousePos)) {
                        gameOverMenuButton.setFillColor(sf::Color(100, 100, 150));
                    } else {
                        gameOverMenuButton.setFillColor(sf::Color(70, 70, 100));
                    }
                    
                    if (IsMouseOver(gameOverRetryButton, mousePos)) {
                        gameOverRetryButton.setFillColor(sf::Color(100, 100, 150));
                    } else {
                        gameOverRetryButton.setFillColor(sf::Color(70, 70, 100));
                    }
                    
                    if (IsMouseOver(gameOverExitButton, mousePos)) {
                        gameOverExitButton.setFillColor(sf::Color(100, 100, 150));
                    } else {
                        gameOverExitButton.setFillColor(sf::Color(70, 70, 100));
                    }
                    
                    window.draw(gameOverMenuButton);
                    window.draw(gameOverMenuText);
                    window.draw(gameOverRetryButton);
                    window.draw(gameOverRetryText);
                    window.draw(gameOverExitButton);
                    window.draw(gameOverExitText);
                }
            }
        }
        
        window.display();
    }
    
    return 0;
}
