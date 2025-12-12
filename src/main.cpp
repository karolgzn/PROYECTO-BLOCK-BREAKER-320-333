#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Block.hpp"
#include "Boss.hpp"
#include "FallingBlock.hpp"
#include "PowerUp.hpp"

// Estados del juego
enum GameState {
    MENU,
    PLAYING,
    BOSS_FIGHT,
    CONTROLS,
    SKIN_SELECTION,
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

// Constantes del Boss
const int BOSS_TRIGGER_SCORE = 1000;
const float BOSS_DAMAGE_PER_HIT = 10.0f;
const int MAX_FALLING_BLOCKS = 5;
const float PADDLE_SLOW_DURATION = 2.0f;
const float PADDLE_SLOW_PERCENTAGE = 0.6f;

// Funcion para crear un boton visual
void CreateButton(sf::RectangleShape& button, sf::Text& text, const std::string& label, 
                  float x, float y, float width, float height, const sf::Font& font) {
    button.setSize(sf::Vector2f(width, height));
    button.setPosition(x, y);
    button.setFillColor(sf::Color(25, 25, 80));  // Azul oscuro retro
    button.setOutlineThickness(5);  // Borde mas grueso estilo retro
    button.setOutlineColor(sf::Color(0, 255, 255));  // Cyan brillante
    
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(28);  // Texto mas grande
    text.setFillColor(sf::Color(255, 255, 0));  // Amarillo brillante
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, 
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

// Funcion para verificar si el mouse esta sobre un boton
bool IsMouseOver(const sf::RectangleShape& button, const sf::Vector2i& mousePos) {
    return button.getGlobalBounds().contains(static_cast<float>(mousePos.x), 
                                            static_cast<float>(mousePos.y));
}

// Funcion para verificar colision circulo-rectangulo
bool CheckCollision(const Ball& ball, const sf::FloatRect& rect, bool& hitTop) {
    sf::Vector2f ballPos = ball.GetPosition();
    float radius = ball.GetRadius();
    
    // Encontrar el punto mas cercano del rectangulo al centro de la pelota
    float closestX = std::max(rect.left, std::min(ballPos.x, rect.left + rect.width));
    float closestY = std::max(rect.top, std::min(ballPos.y, rect.top + rect.height));
    
    // Calcular distancia entre el punto mas cercano y el centro de la pelota
    float distanceX = ballPos.x - closestX;
    float distanceY = ballPos.y - closestY;
    float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    
    // Verificar si hay colision
    if (distanceSquared < (radius * radius)) {
        // Determinar si golpeo arriba/abajo o los lados
        hitTop = std::abs(distanceY) > std::abs(distanceX);
        return true;
    }
    
    return false;
}

int main() {
    // Cargar texturas de powerups
    PowerUp::LoadTextures();
    
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
    const float MAX_BALL_SPEED = 675.0f; // 125% mas rapido (2.25x velocidad inicial)
    bool paddleSpeedBoosted = false;
    int selectedSkin = 2; // 0=hotdog, 1=laboratorio, 2=original (default), 3=pan, 4=sable, 5=trampolin
    
    // Crear objetos del juego
    Ball ball(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, BALL_RADIUS);
    Paddle paddle(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 50.0f, PADDLE_WIDTH, PADDLE_HEIGHT);
    
    // Boss y bloques cayentes
    Boss boss(WINDOW_WIDTH / 2.0f, 100.0f);
    std::vector<FallingBlock> fallingBlocks;
    bool bossDefeated = false;
    bool bossTriggered = false;
    int lastBossScore = 0;
    
    // PowerUps
    std::vector<PowerUp> powerUps;
    
    // Multiples pelotas
    std::vector<Ball> extraBalls;
    
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
    
    // Configurar texto con multiples opciones de fuente
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
        std::cout << "Advertencia: No se pudo cargar fuente. El texto no se mostrara." << std::endl;
    }
    
    sf::Text scoreText;
    sf::Text messageText;
    
    // Textura y sprites para corazones de vidas
    sf::Texture heartTexture;
    std::vector<sf::Sprite> heartSprites;
    bool heartLoaded = heartTexture.loadFromFile("assets/corazon.png");
    if (!heartLoaded) {
        std::cerr << "Error cargando corazon.png, usando fallback" << std::endl;
        sf::Image heartImg;
        heartImg.create(20, 20, sf::Color::Red);
        heartTexture.loadFromImage(heartImg);
    }
    
    // Textura de fondo para el juego
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool backgroundLoaded = backgroundTexture.loadFromFile("assets/fondo.png");
    if (backgroundLoaded) {
        backgroundSprite.setTexture(backgroundTexture);
        // Escalar al tamaño de la ventana
        float scaleX = (float)WINDOW_WIDTH / backgroundTexture.getSize().x;
        float scaleY = (float)WINDOW_HEIGHT / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
    } else {
        std::cerr << "Error cargando fondo.png" << std::endl;
    }
    
    // Texturas de preview de skins
    std::vector<sf::Texture> skinTextures(6);
    std::vector<sf::Sprite> skinSprites(6);
    std::vector<std::string> skinNames = {"Hotdog", "Laboratorio", "Original", "Pan", "Sable", "Trampolin"};
    std::vector<std::string> skinFiles = {
        "assets/hotdog.png",
        "assets/laboratorio.png", 
        "assets/original.png",
        "assets/pan.png",
        "assets/sable.png",
        "assets/trampolin.png"
    };
    
    for (int i = 0; i < 6; i++) {
        if (skinTextures[i].loadFromFile(skinFiles[i])) {
            skinSprites[i].setTexture(skinTextures[i]);
            // Escalar a 150px ancho x 15px alto
            float scaleX = 150.0f / skinTextures[i].getSize().x;
            float scaleY = 15.0f / skinTextures[i].getSize().y;
            skinSprites[i].setScale(scaleX, scaleY);
        }
    }
    
    if (fontLoaded) {
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        
        messageText.setFont(font);
        messageText.setCharacterSize(24);
        messageText.setFillColor(sf::Color::White);
    }
    
    // Elementos del menu
    sf::Text titleText;
    sf::RectangleShape playButton, controlsButton, skinsButton, exitButton;
    sf::Text playText, controlsText, skinsText, exitText;
    sf::RectangleShape backButton;
    sf::Text backText;
    
    // Botones de Game Over
    sf::RectangleShape gameOverMenuButton, gameOverRetryButton, gameOverExitButton;
    sf::Text gameOverMenuText, gameOverRetryText, gameOverExitText;
    
    if (fontLoaded) {
        // Titulo del juego
        titleText.setFont(font);
        titleText.setString("ARKANOID");
        titleText.setCharacterSize(96);  // Titulo mas grande
        titleText.setFillColor(sf::Color(255, 0, 255));  // Magenta brillante
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineThickness(5);  // Borde mas grueso
        titleText.setOutlineColor(sf::Color(255, 255, 0));  // Amarillo brillante
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f, 
                           titleBounds.top + titleBounds.height / 2.0f);
        titleText.setPosition(WINDOW_WIDTH / 2.0f, 100);
        
        // Crear botones del menu - mas grandes y espaciados
        CreateButton(playButton, playText, "INICIAR JUEGO", 
                    WINDOW_WIDTH / 2.0f - 200, 220, 400, 70, font);
        CreateButton(controlsButton, controlsText, "CONTROLES", 
                    WINDOW_WIDTH / 2.0f - 200, 310, 400, 70, font);
        CreateButton(skinsButton, skinsText, "SKINS", 
                    WINDOW_WIDTH / 2.0f - 200, 400, 400, 70, font);
        CreateButton(exitButton, exitText, "SALIR", 
                    WINDOW_WIDTH / 2.0f - 200, 490, 400, 70, font);
        
        // Boton de regresar (para pantalla de controles)
        CreateButton(backButton, backText, "REGRESAR", 
                    WINDOW_WIDTH / 2.0f - 120, 500, 240, 50, font);
        
        // Botones de Game Over - 2 arriba muy juntos, 1 abajo mas grande
        CreateButton(gameOverMenuButton, gameOverMenuText, "MENU", 
                    WINDOW_WIDTH / 2.0f - 230, 320, 220, 80, font);
        CreateButton(gameOverExitButton, gameOverExitText, "SALIR", 
                    WINDOW_WIDTH / 2.0f + 10, 320, 220, 80, font);
        CreateButton(gameOverRetryButton, gameOverRetryText, "REINTENTAR", 
                    WINDOW_WIDTH / 2.0f - 160, 430, 320, 85, font);
    }
    
    // SISTEMA DE MUSICA - Menu y Juego separados
    sf::Music menuMusic;
    sf::Music gameMusic;
    bool menuMusicLoaded = false;
    bool gameMusicLoaded = false;
    GameState currentMusicState = MENU;
    
    // Cargar musica del menu
    if (menuMusic.openFromFile("assets/music/menu_music.ogg")) {
        menuMusicLoaded = true;
        menuMusic.setLoop(true);
        menuMusic.setVolume(45.0f);
        std::cout << "Musica del menu cargada" << std::endl;
    }
    else if (menuMusic.openFromFile("assets/music/menu_music.wav")) {
        menuMusicLoaded = true;
        menuMusic.setLoop(true);
        menuMusic.setVolume(45.0f);
        std::cout << "Musica del menu cargada" << std::endl;
    }
    else {
        std::cout << "Advertencia: No se encontro musica del menu (menu_music.ogg)" << std::endl;
    }
    
    // Cargar musica del juego
    if (gameMusic.openFromFile("assets/music/game_music.ogg")) {
        gameMusicLoaded = true;
        gameMusic.setLoop(true);
        gameMusic.setVolume(50.0f);
        std::cout << "Musica del juego cargada" << std::endl;
    }
    else if (gameMusic.openFromFile("assets/music/game_music.wav")) {
        gameMusicLoaded = true;
        gameMusic.setLoop(true);
        gameMusic.setVolume(50.0f);
        std::cout << "Musica del juego cargada" << std::endl;
    }
    else {
        std::cout << "Advertencia: No se encontro musica del juego (game_music.ogg)" << std::endl;
    }
    
    // Reproducir musica del menu al inicio
    if (menuMusicLoaded) {
        menuMusic.play();
        currentMusicState = MENU;
    }
    
    // SISTEMA DE EFECTOS DE SONIDO
    sf::SoundBuffer paddleHitBuffer, blockHitBuffer, wallHitBuffer, powerUpBuffer;
    sf::SoundBuffer loseLifeBuffer, gameOverBuffer, victoryBuffer, bossHitBuffer, bossDefeatedBuffer;
    sf::Sound paddleHitSound, blockHitSound, wallHitSound, powerUpSound;
    sf::Sound loseLifeSound, gameOverSound, victorySound, bossHitSound, bossDefeatedSound;
    
    // Cargar efectos de sonido
    if (paddleHitBuffer.loadFromFile("assets/sounds/paddle_hit.wav")) {
        paddleHitSound.setBuffer(paddleHitBuffer);
        paddleHitSound.setVolume(70.0f);
    }
    if (blockHitBuffer.loadFromFile("assets/sounds/block_hit.wav")) {
        blockHitSound.setBuffer(blockHitBuffer);
        blockHitSound.setVolume(60.0f);
    }
    if (wallHitBuffer.loadFromFile("assets/sounds/wall_hit.wav")) {
        wallHitSound.setBuffer(wallHitBuffer);
        wallHitSound.setVolume(50.0f);
    }
    if (powerUpBuffer.loadFromFile("assets/sounds/powerup.wav")) {
        powerUpSound.setBuffer(powerUpBuffer);
        powerUpSound.setVolume(80.0f);
    }
    if (loseLifeBuffer.loadFromFile("assets/sounds/lose_life.wav")) {
        loseLifeSound.setBuffer(loseLifeBuffer);
        loseLifeSound.setVolume(90.0f);
    }
    if (gameOverBuffer.loadFromFile("assets/sounds/game_over.wav")) {
        gameOverSound.setBuffer(gameOverBuffer);
        gameOverSound.setVolume(80.0f);
    }
    if (victoryBuffer.loadFromFile("assets/sounds/victory.wav")) {
        victorySound.setBuffer(victoryBuffer);
        victorySound.setVolume(80.0f);
    }
    if (bossHitBuffer.loadFromFile("assets/sounds/boss_hit.wav")) {
        bossHitSound.setBuffer(bossHitBuffer);
        bossHitSound.setVolume(75.0f);
    }
    if (bossDefeatedBuffer.loadFromFile("assets/sounds/boss_defeated.wav")) {
        bossDefeatedSound.setBuffer(bossDefeatedBuffer);
        bossDefeatedSound.setVolume(90.0f);
    }
    
    std::cout << "Sistema de sonido inicializado" << std::endl;
    
    // Reloj para delta time
    sf::Clock clock;
    float bossMessageTimer = 0.0f;
    bool showBossMessage = false;
    
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
                        paddle.SetSkin(selectedSkin);  // Aplicar la skin seleccionada
                        std::cout << "Juego iniciado desde el menu" << std::endl;
                        // Cambiar a musica del juego
                        if (menuMusicLoaded) menuMusic.stop();
                        if (gameMusicLoaded) gameMusic.play();
                        currentMusicState = PLAYING;
                    }
                    else if (IsMouseOver(controlsButton, mousePos)) {
                        gameState = CONTROLS;
                        std::cout << "Mostrando controles" << std::endl;
                    }
                    else if (IsMouseOver(skinsButton, mousePos)) {
                        gameState = SKIN_SELECTION;
                        std::cout << "Seleccion de skins" << std::endl;
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
                else if (gameState == SKIN_SELECTION) {
                    // Botones de skins (3 arriba, 3 abajo)
                    for (int i = 0; i < 6; i++) {
                        int row = i / 3;
                        int col = i % 3;
                        float x = 130 + col * 200;
                        float y = 180 + row * 180;
                        sf::FloatRect skinArea(x, y, 150, 120);
                        if (skinArea.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            selectedSkin = i;
                            paddle.SetSkin(selectedSkin);
                            std::cout << "Skin seleccionada: " << i << std::endl;
                        }
                    }
                    if (IsMouseOver(backButton, mousePos)) {
                        gameState = MENU;
                    }
                }
                else if (gameState == PLAYING && gameOver) {
                    // Manejo de clics en pantalla de Game Over
                    if (IsMouseOver(gameOverMenuButton, mousePos)) {
                        // Volver al menu
                        gameState = MENU;
                        gameStarted = false;
                        gameOver = false;
                        victory = false;
                        score = 0;
                        lives = 3;
                        lastSpeedIncreaseScore = 0;
                        paddleSpeedBoosted = false;
                        bossTriggered = false;
                        bossDefeated = false;
                        fallingBlocks.clear();
                        boss = Boss(WINDOW_WIDTH / 2.0f, 80.0f);
                        ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                        ball.ResetSpeed();
                        // Volver a musica del menu
                        if (gameMusicLoaded) gameMusic.stop();
                        if (menuMusicLoaded) menuMusic.play();
                        currentMusicState = MENU;
                        
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
                        paddleSpeedBoosted = false;
                        bossTriggered = false;
                        bossDefeated = false;
                        fallingBlocks.clear();
                        boss = Boss(WINDOW_WIDTH / 2.0f, 80.0f);
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
                else if (gameState == BOSS_FIGHT && gameOver) {
                    // Manejo de clics en pantalla de Game Over para Boss Fight
                    if (IsMouseOver(gameOverMenuButton, mousePos)) {
                        // Volver al menu
                        gameState = MENU;
                        gameStarted = false;
                        gameOver = false;
                        victory = false;
                        score = 0;
                        lives = 3;
                        lastSpeedIncreaseScore = 0;
                        paddleSpeedBoosted = false;
                        bossTriggered = false;
                        bossDefeated = false;
                        fallingBlocks.clear();
                        boss = Boss(WINDOW_WIDTH / 2.0f, 80.0f);
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
                        paddleSpeedBoosted = false;
                        bossTriggered = false;
                        bossDefeated = false;
                        fallingBlocks.clear();
                        boss = Boss(WINDOW_WIDTH / 2.0f, 80.0f);
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
                if (event.key.code == sf::Keyboard::Space && (gameState == PLAYING || gameState == BOSS_FIGHT) && !gameStarted && !gameOver) {
                    gameStarted = true;
                    ball.Launch(-60.0f); // Lanzar hacia arriba-izquierda
                }
                
                // Control de musica con tecla M
                if (event.key.code == sf::Keyboard::M) {
                    if (currentMusicState == MENU && menuMusicLoaded) {
                        if (menuMusic.getStatus() == sf::Music::Playing) {
                            menuMusic.pause();
                            std::cout << "Musica pausada" << std::endl;
                        } else {
                            menuMusic.play();
                            std::cout << "Musica reanudada" << std::endl;
                        }
                    } else if ((currentMusicState == PLAYING || currentMusicState == BOSS_FIGHT) && gameMusicLoaded) {
                        if (gameMusic.getStatus() == sf::Music::Playing) {
                            gameMusic.pause();
                            std::cout << "Musica pausada" << std::endl;
                        } else {
                            gameMusic.play();
                            std::cout << "Musica reanudada" << std::endl;
                        }
                    }
                }
                
                // Regresar al menu desde Game Over
                if (event.key.code == sf::Keyboard::Escape && gameState == PLAYING) {
                    gameState = MENU;
                    gameStarted = false;
                    gameOver = false;
                    // Volver a musica del menu
                    if (gameMusicLoaded) gameMusic.stop();
                    if (menuMusicLoaded) menuMusic.play();
                    currentMusicState = MENU;
                }
                
                if (event.key.code == sf::Keyboard::R && gameOver) {
                    // Reiniciar juego
                    gameState = PLAYING;
                    score = 0;
                    lives = 3;
                    lastSpeedIncreaseScore = 0;
                    paddleSpeedBoosted = false;
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
        
        // Contar bloques restantes
        int blocksRemaining = 0;
        for (const auto& block : blocks) {
            if (!block.IsDestroyed()) blocksRemaining++;
        }
        
        if ((gameState == PLAYING || gameState == BOSS_FIGHT) && !gameOver && gameStarted) {
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
            paddle.UpdatePowerUps(deltaTime);
            
            // Actualizar pelotas extra
            for (auto& extraBall : extraBalls) {
                extraBall.Update(deltaTime);
            }
            
            // Actualizar powerups
            for (auto& powerup : powerUps) {
                powerup.Update(deltaTime);
                
                // Colision powerup con paddle
                if (powerup.GetBounds().intersects(paddle.GetBounds())) {
                    PowerUpType type = powerup.GetType();
                    powerup.Deactivate();
                    powerUpSound.play();
                    
                    switch(type) {
                        case PowerUpType::EXTRA_LIFE:
                            if (lives < 3) {
                                lives++;
                                std::cout << "¡Vida extra! Vidas: " << lives << std::endl;
                            } else {
                                std::cout << "Vidas al maximo (3)" << std::endl;
                            }
                            break;
                        case PowerUpType::PADDLE_EXPAND:
                            paddle.ApplySizeChange(1.5f, 7.0f);
                            std::cout << "¡Paleta expandida por 7 segundos!" << std::endl;
                            break;
                        case PowerUpType::PADDLE_SHRINK:
                            paddle.ApplySizeChange(0.7f, 7.0f);
                            std::cout << "¡Paleta reducida por 7 segundos!" << std::endl;
                            break;
                        case PowerUpType::PADDLE_FAST:
                            paddle.ApplySpeedChange(1.15f, 7.0f);
                            std::cout << "¡Paleta mas rapida por 7 segundos!" << std::endl;
                            break;
                        case PowerUpType::PADDLE_SLOW:
                            paddle.ApplySpeedChange(0.85f, 7.0f);
                            std::cout << "¡Paleta mas lenta por 7 segundos!" << std::endl;
                            break;
                        case PowerUpType::MULTI_BALL:
                            {
                                sf::Vector2f ballPos = ball.GetPosition();
                                sf::Vector2f ballVel = ball.GetVelocity();
                                float currentSpeed = ball.GetSpeed();
                                Ball newBall1(ballPos.x, ballPos.y, BALL_RADIUS);
                                Ball newBall2(ballPos.x, ballPos.y, BALL_RADIUS);
                                newBall1.SetSpeed(currentSpeed);
                                newBall2.SetSpeed(currentSpeed);
                                newBall1.SetVelocity(sf::Vector2f(ballVel.x * 0.8f, ballVel.y * 1.2f));
                                newBall2.SetVelocity(sf::Vector2f(ballVel.x * 1.2f, ballVel.y * 0.8f));
                                extraBalls.push_back(newBall1);
                                extraBalls.push_back(newBall2);
                                std::cout << "¡2 pelotas extra! Total: " << (1 + extraBalls.size()) << std::endl;
                            }
                            break;
                    }
                }
            }
            
            // Limpiar powerups inactivos
            powerUps.erase(
                std::remove_if(powerUps.begin(), powerUps.end(),
                    [](const PowerUp& p) { return !p.IsActive(); }),
                powerUps.end()
            );
            
            // Colisiones con paredes
            sf::Vector2f ballPos = ball.GetPosition();
            
            // Pared izquierda
            if (ballPos.x - BALL_RADIUS < 0) {
                ball.ReverseX();
                ball.SetPosition(BALL_RADIUS, ballPos.y); // Reposicionar fuera de la pared
                wallHitSound.play();
            }
            // Pared derecha
            if (ballPos.x + BALL_RADIUS > WINDOW_WIDTH) {
                ball.ReverseX();
                ball.SetPosition(WINDOW_WIDTH - BALL_RADIUS, ballPos.y); // Reposicionar fuera de la pared
                wallHitSound.play();
            }
            // Pared superior
            if (ballPos.y - BALL_RADIUS < 0) {
                ball.ReverseY();
                ball.SetPosition(ballPos.x, BALL_RADIUS); // Reposicionar fuera de la pared
                wallHitSound.play();
            }
            
            // Pelota cayo
            if (ballPos.y - BALL_RADIUS > WINDOW_HEIGHT) {
                // Solo perder vida si no hay pelotas extra
                if (extraBalls.empty()) {
                    lives--;
                    loseLifeSound.play();
                    std::cout << "Vida perdida! Vidas restantes: " << lives << std::endl;
                    
                    if (lives <= 0) {
                        gameOver = true;
                        victory = false;
                        gameOverSound.play();
                        std::cout << "Game Over! Puntuacion final: " << score << std::endl;
                    } else {
                        ball.Reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                        gameStarted = false;
                    }
                } else {
                    // Si hay pelotas extra, tomar una de ellas como principal
                    ball = extraBalls.back();
                    extraBalls.pop_back();
                    std::cout << "Pelota principal reemplazada. Pelotas restantes: " << (1 + extraBalls.size()) << std::endl;
                }
            }
            
            // Colision con paddle
            bool hitTop;
            if (CheckCollision(ball, paddle.GetBounds(), hitTop)) {
                ball.ReverseY();
                paddleHitSound.play();
                
                // Modificar angulo basado en donde golpeo el paddle
                sf::Vector2f paddlePos = paddle.GetPosition();
                float relativeIntersect = (ballPos.x - paddlePos.x) / (paddle.GetWidth() / 2.0f);
                float angle = relativeIntersect * 60.0f; // Maximo 60 grados
                
                sf::Vector2f velocity = ball.GetVelocity();
                float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                
                float radians = (angle - 90.0f) * 3.14159f / 180.0f;
                ball.SetVelocity(sf::Vector2f(std::cos(radians) * speed, std::sin(radians) * speed));
            }
            
            // Colision con bloques
            for (auto& block : blocks) {
                if (!block.IsDestroyed()) {
                    if (CheckCollision(ball, block.GetBounds(), hitTop)) {
                        sf::Vector2f blockPos = sf::Vector2f(block.GetBounds().left + block.GetBounds().width / 2,
                                                              block.GetBounds().top + block.GetBounds().height / 2);
                        block.Destroy();
                        blockHitSound.play();
                        score += block.GetPoints();
                        std::cout << "Bloque destruido! Puntos: " << score << std::endl;
                        
                        // Probabilidad de generar powerup
                        int random = rand() % 100;
                        if (random < 1 && lives <= 2) { // 1% vida extra (solo si vidas <= 2)
                            powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::EXTRA_LIFE);
                        } else if (random < 5) { // 4% expansion
                            powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_EXPAND);
                        } else if (random < 8) { // 3% contraccion
                            powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_SHRINK);
                        } else if (random < 12) { // 4% velocidad paleta
                            powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_FAST);
                        } else if (random < 15) { // 3% lentitud paleta
                            powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_SLOW);
                        } else if (random < 18) { // 3% multi bola
                            powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::MULTI_BALL);
                        }
                        
                        // Aumentar velocidad cada 70 puntos
                        if (score - lastSpeedIncreaseScore >= 70) {
                            if (ball.GetSpeed() < MAX_BALL_SPEED) {
                                ball.IncreaseSpeed(10.0f); // 10% mas rapido
                                lastSpeedIncreaseScore = score;
                                std::cout << "¡Velocidad de bola aumentada! Velocidad actual: " 
                                         << ball.GetSpeed() << std::endl;
                                
                                // Aumentar velocidad de paleta 15% cuando pelota alcance 700 px/s
                                if (ball.GetSpeed() >= 700.0f && !paddleSpeedBoosted) {
                                    paddle.ApplySpeedChange(1.15f, 999999.0f); // Permanente
                                    paddleSpeedBoosted = true;
                                    std::cout << "¡Paleta mejorada! Velocidad aumentada 15%" << std::endl;
                                }
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
            
            // Colisiones para pelotas extra
            for (auto& extraBall : extraBalls) {
                sf::Vector2f eBallPos = extraBall.GetPosition();
                
                // Paredes
                if (eBallPos.x - BALL_RADIUS < 0) {
                    extraBall.ReverseX();
                    extraBall.SetPosition(BALL_RADIUS, eBallPos.y);
                }
                if (eBallPos.x + BALL_RADIUS > WINDOW_WIDTH) {
                    extraBall.ReverseX();
                    extraBall.SetPosition(WINDOW_WIDTH - BALL_RADIUS, eBallPos.y);
                }
                if (eBallPos.y - BALL_RADIUS < 0) {
                    extraBall.ReverseY();
                    extraBall.SetPosition(eBallPos.x, BALL_RADIUS);
                }
                
                // Paddle
                bool hitTop;
                if (CheckCollision(extraBall, paddle.GetBounds(), hitTop)) {
                    extraBall.ReverseY();
                    sf::Vector2f paddlePos = paddle.GetPosition();
                    float relativeIntersect = (eBallPos.x - paddlePos.x) / (paddle.GetWidth() / 2.0f);
                    float angle = relativeIntersect * 60.0f;
                    sf::Vector2f velocity = extraBall.GetVelocity();
                    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                    float radians = (angle - 90.0f) * 3.14159f / 180.0f;
                    extraBall.SetVelocity(sf::Vector2f(std::cos(radians) * speed, std::sin(radians) * speed));
                }
                
                // Bloques
                for (auto& block : blocks) {
                    if (!block.IsDestroyed()) {
                        if (CheckCollision(extraBall, block.GetBounds(), hitTop)) {
                            sf::Vector2f blockPos = sf::Vector2f(block.GetBounds().left + block.GetBounds().width / 2,
                                                                  block.GetBounds().top + block.GetBounds().height / 2);
                            block.Destroy();
                            score += block.GetPoints();
                            
                            // Generar powerup
                            int random = rand() % 100;
                            if (random < 1 && lives <= 2) {
                                powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::EXTRA_LIFE);
                            } else if (random < 5) {
                                powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_EXPAND);
                            } else if (random < 8) {
                                powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_SHRINK);
                            } else if (random < 12) {
                                powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_FAST);
                            } else if (random < 15) {
                                powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::PADDLE_SLOW);
                            } else if (random < 18) {
                                powerUps.emplace_back(blockPos.x, blockPos.y, PowerUpType::MULTI_BALL);
                            }
                            
                            if (hitTop) {
                                extraBall.ReverseY();
                            } else {
                                extraBall.ReverseX();
                            }
                            break;
                        }
                    }
                }
            }
            
            // Eliminar pelotas extra que cayeron
            extraBalls.erase(
                std::remove_if(extraBalls.begin(), extraBalls.end(),
                    [](const Ball& b) { return b.GetPosition().y - BALL_RADIUS > WINDOW_HEIGHT; }),
                extraBalls.end()
            );
            
            // Verificar si debe aparecer el boss (cada 1000 puntos)
            if ((score - lastBossScore) >= BOSS_TRIGGER_SCORE && !bossTriggered && blocksRemaining == 0) {
                bossTriggered = true;
                gameState = BOSS_FIGHT;
                showBossMessage = true;
                bossMessageTimer = 2.0f;
                std::cout << "¡JEFE APARECIO! Preparate para la batalla final!" << std::endl;
            }
            
            // Recrear bloques si se terminaron y no hay boss
            if (blocksRemaining == 0 && !bossTriggered && (score - lastBossScore) < BOSS_TRIGGER_SCORE) {
                blocks.clear();
                for (int row = 0; row < BLOCK_ROWS; ++row) {
                    for (int col = 0; col < BLOCKS_PER_ROW; ++col) {
                        float x = startX + col * BLOCK_WIDTH;
                        float y = startY + row * (BLOCK_HEIGHT + 5.0f);
                        blocks.emplace_back(x, y, BLOCK_WIDTH - 5.0f, BLOCK_HEIGHT, colors[row], 10);
                    }
                }
                std::cout << "¡Bloques recreados! Continua jugando..." << std::endl;
            }
            
            // Verificar victoria (solo si ya derroto al boss)
            if (blocksRemaining == 0 && !bossTriggered) {
                // Activar boss si llega a 500 puntos
                if (score >= BOSS_TRIGGER_SCORE) {
                    bossTriggered = true;
                    gameState = BOSS_FIGHT;
                    std::cout << "¡JEFE APARECIO!" << std::endl;
                }
            }
        }
        
        // Logica del Boss Fight
        if (gameState == BOSS_FIGHT && !gameOver && gameStarted) {
            // Actualizar temporizador de mensaje
            if (showBossMessage) {
                bossMessageTimer -= deltaTime;
                if (bossMessageTimer <= 0.0f) {
                    showBossMessage = false;
                }
            }
            
            // Actualizar boss
            boss.Update(deltaTime, WINDOW_WIDTH);
            
            // Boss dispara bloques cayentes
            if (boss.ShouldShoot() && boss.IsAlive()) {
                if (fallingBlocks.size() < MAX_FALLING_BLOCKS) {
                    sf::Vector2f shootPos = boss.GetShootPosition();
                    fallingBlocks.emplace_back(shootPos.x, shootPos.y);
                    boss.ResetShootTimer();
                    std::cout << "Boss disparo un bloque!" << std::endl;
                }
            }
            
            // Actualizar bloques cayentes
            for (auto& fb : fallingBlocks) {
                if (fb.IsActive()) {
                    fb.Update(deltaTime);
                    
                    // Colision bloque cayente vs paddle
                    if (fb.GetBounds().intersects(paddle.GetBounds())) {
                        paddle.ApplySlow(PADDLE_SLOW_DURATION, PADDLE_SLOW_PERCENTAGE);
                        fb.Deactivate();
                        std::cout << "¡Paddle ralentizado por 3 segundos!" << std::endl;
                    }
                }
            }
            
            // Limpiar bloques cayentes inactivos
            fallingBlocks.erase(
                std::remove_if(fallingBlocks.begin(), fallingBlocks.end(),
                    [](const FallingBlock& fb) { return !fb.IsActive(); }),
                fallingBlocks.end()
            );
            
            // Colision pelota vs boss
            bool hitTop;
            if (boss.IsAlive() && CheckCollision(ball, boss.GetBounds(), hitTop)) {
                boss.TakeDamage(BOSS_DAMAGE_PER_HIT);
                bossHitSound.play();
                ball.ReverseY();
                
                // Reposicionar pelota ligeramente fuera del boss para colision mas natural
                sf::FloatRect bossBounds = boss.GetBounds();
                sf::Vector2f ballPos = ball.GetPosition();
                if (hitTop) {
                    ball.SetPosition(ballPos.x, bossBounds.top + bossBounds.height + BALL_RADIUS + 0.5f);
                } else {
                    ball.SetPosition(ballPos.x, bossBounds.top - BALL_RADIUS - 0.5f);
                }
                
                std::cout << "¡Boss golpeado! Vida: " << boss.GetHealth() << std::endl;
                
                // Continuar juego cuando boss es derrotado
                if (!boss.IsAlive() && !bossDefeated) {
                    bossDefeated = true;
                    bossDefeatedSound.play();
                    score += 50; // Bonus por derrotar al boss
                    lastBossScore = score; // Marcar para proximo boss en +1000 puntos
                    gameState = PLAYING;
                    fallingBlocks.clear();
                    
                    // Resetear boss para proxima aparicion
                    bossTriggered = false;
                    bossDefeated = false;
                    boss = Boss(WINDOW_WIDTH / 2.0f, 100.0f);
                    
                    std::cout << "¡JEFE DERROTADO! +50 puntos. El juego continua..." << std::endl;
                    std::cout << "Proximo jefe aparecera en: " << (lastBossScore + BOSS_TRIGGER_SCORE) << " puntos" << std::endl;
                    
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
        
        // Si no ha empezado, la pelota sigue al paddle
        if ((gameState == PLAYING || gameState == BOSS_FIGHT) && !gameStarted && !gameOver) {
            sf::Vector2f paddlePos = paddle.GetPosition();
            ball.Reset(paddlePos.x, paddlePos.y - 30.0f);
        }
        
        // Actualizar textos
        if (fontLoaded) {
            scoreText.setString("Puntuacion: " + std::to_string(score));
        }
        
        // Actualizar corazones
        heartSprites.clear();
        for (int i = 0; i < lives; i++) {
            sf::Sprite heart;
            heart.setTexture(heartTexture);
            // Escalar a 25x25
            float scale = 25.0f / heartTexture.getSize().x;
            heart.setScale(scale, scale);
            heart.setPosition(WINDOW_WIDTH - 40 - (i * 30), 10);
            heartSprites.push_back(heart);
        }
        
        // Renderizado
        window.clear(sf::Color(10, 0, 40));  // Morado oscuro retro
        
        if (gameState == MENU) {
            // Fondo retro con patrón de gradiente pixelado
            for (int y = 0; y < 600; y += 20) {
                for (int x = 0; x < 800; x += 20) {
                    sf::RectangleShape pixel(sf::Vector2f(20, 20));
                    pixel.setPosition(x, y);
                    // Gradiente de colores estilo 16-bit
                    int colorIndex = (x / 80 + y / 60) % 4;
                    if (colorIndex == 0) pixel.setFillColor(sf::Color(20, 10, 60));
                    else if (colorIndex == 1) pixel.setFillColor(sf::Color(30, 10, 70));
                    else if (colorIndex == 2) pixel.setFillColor(sf::Color(25, 5, 65));
                    else pixel.setFillColor(sf::Color(15, 15, 55));
                    window.draw(pixel);
                }
            }
            
            // Borde decorativo superior estilo 16-bit
            for (int i = 0; i < 800; i += 40) {
                sf::RectangleShape topBar(sf::Vector2f(35, 15));
                topBar.setPosition(i, 0);
                topBar.setFillColor(i % 80 == 0 ? sf::Color(255, 0, 255) : sf::Color(0, 255, 255));
                window.draw(topBar);
            }
            
            // Borde decorativo inferior
            for (int i = 0; i < 800; i += 40) {
                sf::RectangleShape bottomBar(sf::Vector2f(35, 15));
                bottomBar.setPosition(i, 585);
                bottomBar.setFillColor(i % 80 == 0 ? sf::Color(0, 255, 255) : sf::Color(255, 0, 255));
                window.draw(bottomBar);
            }
            
            // Dibujar menu principal
            if (fontLoaded) {
                window.draw(titleText);
                
                // Efecto hover en botones
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                
                if (IsMouseOver(playButton, mousePos)) {
                    playButton.setFillColor(sf::Color(255, 0, 255));  // Magenta brillante hover
                    playButton.setOutlineThickness(5);
                    playButton.setOutlineColor(sf::Color(255, 255, 0));  // Borde amarillo
                } else {
                    playButton.setFillColor(sf::Color(25, 25, 80));  // Azul oscuro retro
                    playButton.setOutlineThickness(4);
                    playButton.setOutlineColor(sf::Color(0, 255, 255));  // Borde cyan
                }
                
                if (IsMouseOver(controlsButton, mousePos)) {
                    controlsButton.setFillColor(sf::Color(255, 0, 255));  // Magenta brillante hover
                    controlsButton.setOutlineThickness(5);
                    controlsButton.setOutlineColor(sf::Color(255, 255, 0));
                } else {
                    controlsButton.setFillColor(sf::Color(25, 25, 80));
                    controlsButton.setOutlineThickness(4);
                    controlsButton.setOutlineColor(sf::Color(0, 255, 255));
                }
                
                if (IsMouseOver(skinsButton, mousePos)) {
                    skinsButton.setFillColor(sf::Color(255, 0, 255));  // Magenta brillante hover
                    skinsButton.setOutlineThickness(5);
                    skinsButton.setOutlineColor(sf::Color(255, 255, 0));
                } else {
                    skinsButton.setFillColor(sf::Color(25, 25, 80));
                    skinsButton.setOutlineThickness(4);
                    skinsButton.setOutlineColor(sf::Color(0, 255, 255));
                }
                
                if (IsMouseOver(exitButton, mousePos)) {
                    exitButton.setFillColor(sf::Color(255, 0, 255));  // Magenta brillante hover
                    exitButton.setOutlineThickness(5);
                    exitButton.setOutlineColor(sf::Color(255, 255, 0));
                } else {
                    exitButton.setFillColor(sf::Color(25, 25, 80));
                    exitButton.setOutlineThickness(4);
                    exitButton.setOutlineColor(sf::Color(0, 255, 255));
                }
                
                // Sombras de botones (efecto 16-bit)
                sf::RectangleShape playShadow(playButton.getSize());
                playShadow.setPosition(playButton.getPosition().x + 6, playButton.getPosition().y + 6);
                playShadow.setFillColor(sf::Color(0, 0, 0, 100));
                window.draw(playShadow);
                
                sf::RectangleShape controlsShadow(controlsButton.getSize());
                controlsShadow.setPosition(controlsButton.getPosition().x + 6, controlsButton.getPosition().y + 6);
                controlsShadow.setFillColor(sf::Color(0, 0, 0, 100));
                window.draw(controlsShadow);
                
                sf::RectangleShape skinsShadow(skinsButton.getSize());
                skinsShadow.setPosition(skinsButton.getPosition().x + 6, skinsButton.getPosition().y + 6);
                skinsShadow.setFillColor(sf::Color(0, 0, 0, 100));
                window.draw(skinsShadow);
                
                sf::RectangleShape exitShadow(exitButton.getSize());
                exitShadow.setPosition(exitButton.getPosition().x + 6, exitButton.getPosition().y + 6);
                exitShadow.setFillColor(sf::Color(0, 0, 0, 100));
                window.draw(exitShadow);
                
                window.draw(playButton);
                window.draw(playText);
                window.draw(controlsButton);
                window.draw(controlsText);
                window.draw(skinsButton);
                window.draw(skinsText);
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
                controlsTitle.setCharacterSize(64);  // Titulo mas grande
                controlsTitle.setFillColor(sf::Color(255, 0, 255));  // Magenta retro
                controlsTitle.setOutlineThickness(3);
                controlsTitle.setOutlineColor(sf::Color(0, 255, 255));  // Cyan
                sf::FloatRect ctBounds = controlsTitle.getLocalBounds();
                controlsTitle.setOrigin(ctBounds.left + ctBounds.width / 2.0f, 
                                       ctBounds.top + ctBounds.height / 2.0f);
                controlsTitle.setPosition(WINDOW_WIDTH / 2.0f, 80);
                window.draw(controlsTitle);
                
                sf::Text controlsList;
                controlsList.setFont(font);
                controlsList.setCharacterSize(16);
                controlsList.setFillColor(sf::Color(255, 255, 0));  // Amarillo brillante
                controlsList.setString(
                    "FLECHA IZQUIERDA: Mover paleta a la izquierda\n\n"
                    "FLECHA DERECHA: Mover paleta a la derecha\n\n"
                    "ESPACIO: Lanzar pelota\n\n"
                    "M: Pausar/Reanudar musica\n\n"
                    "R: Reiniciar juego (en Game Over)\n\n"
                    "ESC: Volver al menu"
                );
                sf::FloatRect clBounds = controlsList.getLocalBounds();
                controlsList.setOrigin(clBounds.left + clBounds.width / 2.0f, 
                                       clBounds.top + clBounds.height / 2.0f);
                controlsList.setPosition(WINDOW_WIDTH / 2.0f, 300);
                window.draw(controlsList);
                
                // Boton de regresar con hover
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (IsMouseOver(backButton, mousePos)) {
                    backButton.setFillColor(sf::Color(255, 0, 255));  // Magenta hover
                } else {
                    backButton.setFillColor(sf::Color(25, 25, 80));  // Azul oscuro retro
                }
                
                window.draw(backButton);
                window.draw(backText);
            }
        }
        else if (gameState == SKIN_SELECTION) {
            // Dibujar pantalla de selección de skins
            if (fontLoaded) {
                sf::Text skinsTitle;
                skinsTitle.setFont(font);
                skinsTitle.setString("SELECCIONA TU SKIN");
                skinsTitle.setCharacterSize(40);
                skinsTitle.setFillColor(sf::Color(255, 0, 255));  // Magenta retro
                skinsTitle.setOutlineThickness(3);
                skinsTitle.setOutlineColor(sf::Color(0, 255, 255));  // Cyan
                sf::FloatRect stBounds = skinsTitle.getLocalBounds();
                skinsTitle.setOrigin(stBounds.left + stBounds.width / 2.0f, 
                                     stBounds.top + stBounds.height / 2.0f);
                skinsTitle.setPosition(WINDOW_WIDTH / 2.0f, 80);
                window.draw(skinsTitle);
                
                // Dibujar las 6 cajas de preview de skins (3 columnas, 2 filas)
                for (int i = 0; i < 6; i++) {
                    int row = i / 3;
                    int col = i % 3;
                    float x = 130 + col * 200;
                    float y = 180 + row * 180;
                    
                    // Caja de preview
                    sf::RectangleShape previewBox(sf::Vector2f(150, 120));
                    previewBox.setPosition(x, y);
                    
                    // Highlight para la skin seleccionada
                    if (i == selectedSkin) {
                        previewBox.setFillColor(sf::Color(50, 50, 150));  // Azul oscuro
                        previewBox.setOutlineThickness(4);
                        previewBox.setOutlineColor(sf::Color(0, 255, 255));  // Cyan brillante
                    } else {
                        previewBox.setFillColor(sf::Color(25, 25, 80));
                        previewBox.setOutlineThickness(2);
                        previewBox.setOutlineColor(sf::Color(100, 100, 150));
                    }
                    
                    window.draw(previewBox);
                    
                    // Dibujar sprite de la skin centrado
                    if (i < skinSprites.size() && skinSprites[i].getTexture() != nullptr) {
                        sf::FloatRect spriteBounds = skinSprites[i].getLocalBounds();
                        skinSprites[i].setOrigin(spriteBounds.width / 2.0f, spriteBounds.height / 2.0f);
                        skinSprites[i].setPosition(x + 75, y + 45);
                        window.draw(skinSprites[i]);
                    }
                    
                    // Nombre de la skin
                    sf::Text skinName;
                    skinName.setFont(font);
                    skinName.setString(skinNames[i]);
                    skinName.setCharacterSize(18);
                    skinName.setFillColor(sf::Color(255, 255, 0));  // Amarillo
                    sf::FloatRect nameBounds = skinName.getLocalBounds();
                    skinName.setOrigin(nameBounds.left + nameBounds.width / 2.0f, 
                                       nameBounds.top + nameBounds.height / 2.0f);
                    skinName.setPosition(x + 75, y + 95);
                    window.draw(skinName);
                }
                
                // Botón de regresar con hover
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (IsMouseOver(backButton, mousePos)) {
                    backButton.setFillColor(sf::Color(255, 0, 255));  // Magenta hover
                } else {
                    backButton.setFillColor(sf::Color(25, 25, 80));  // Azul oscuro retro
                }
                
                window.draw(backButton);
                window.draw(backText);
            }
        }
        else if (gameState == PLAYING) {
            // Dibujar fondo
            if (backgroundLoaded) {
                window.draw(backgroundSprite);
            }
            
            // Dibujar objetos del juego
            paddle.Draw(window);
            ball.Draw(window);
            
            // Dibujar pelotas extra
            for (auto& extraBall : extraBalls) {
                extraBall.Draw(window);
            }
            
            // Dibujar powerups
            for (auto& powerup : powerUps) {
                powerup.Draw(window);
            }
            
            for (auto& block : blocks) {
                block.Draw(window);
            }
            
            // Dibujar UI del juego
            if (fontLoaded) {
                window.draw(scoreText);
                
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
            }
            
            // Dibujar corazones de vidas
            for (auto& heart : heartSprites) {
                window.draw(heart);
            }
            
            if (fontLoaded) {
                if (gameOver) {
                    // Overlay oscuro semi-transparente
                    sf::RectangleShape overlay;
                    overlay.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                    overlay.setPosition(0, 0);
                    overlay.setFillColor(sf::Color(0, 0, 0, 200)); // Negro con 200/255 de opacidad
                    window.draw(overlay);
                    
                    // Titulo de Game Over o Victoria
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
                    
                    // Mostrar puntuacion final
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
        else if (gameState == BOSS_FIGHT) {
            // Dibujar fondo
            if (backgroundLoaded) {
                window.draw(backgroundSprite);
            }
            
            // Dibujar objetos del juego en modo Boss
            paddle.Draw(window);
            ball.Draw(window);
            boss.Draw(window);
            
            // Dibujar pelotas extra
            for (auto& extraBall : extraBalls) {
                extraBall.Draw(window);
            }
            
            // Dibujar powerups
            for (auto& powerup : powerUps) {
                powerup.Draw(window);
            }
            
            // Dibujar bloques cayentes
            for (auto& fb : fallingBlocks) {
                fb.Draw(window);
            }
            
            // Dibujar UI
            if (fontLoaded) {
                window.draw(scoreText);
                
                if (!gameStarted && !gameOver) {
                    messageText.setString("Presiona ESPACIO para iniciar");
                    messageText.setCharacterSize(24);
                    messageText.setFillColor(sf::Color::Yellow);
                    sf::FloatRect msgBounds = messageText.getLocalBounds();
                    messageText.setOrigin(msgBounds.left + msgBounds.width / 2.0f, 
                                         msgBounds.top + msgBounds.height / 2.0f);
                    messageText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 100);
                    window.draw(messageText);
                }
                
                // Mostrar mensaje de Boss Fight
                if (!gameOver && showBossMessage) {
                    messageText.setString("¡JEFE APARECIO!");
                    messageText.setCharacterSize(48);
                    messageText.setFillColor(sf::Color::Red);
                    messageText.setStyle(sf::Text::Bold);
                    sf::FloatRect msgBounds = messageText.getLocalBounds();
                    messageText.setOrigin(msgBounds.left + msgBounds.width / 2.0f, 
                                         msgBounds.top + msgBounds.height / 2.0f);
                    messageText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
                    window.draw(messageText);
                    messageText.setStyle(sf::Text::Regular);
                }
            }
            
            // Dibujar corazones de vidas
            for (auto& heart : heartSprites) {
                window.draw(heart);
            }
            
            if (fontLoaded) {
                // Game Over en Boss Fight
                if (gameOver) {
                    if (victory) {
                        messageText.setString("¡JEFE DERROTADO!");
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
                    
                    // Puntuacion final
                    messageText.setString("Puntuacion Final: " + std::to_string(score));
                    messageText.setFillColor(sf::Color::White);
                    messageText.setCharacterSize(32);
                    messageText.setStyle(sf::Text::Regular);
                    msgBounds = messageText.getLocalBounds();
                    messageText.setOrigin(msgBounds.left + msgBounds.width / 2.0f, 
                                         msgBounds.top + msgBounds.height / 2.0f);
                    messageText.setPosition(WINDOW_WIDTH / 2.0f, 240);
                    window.draw(messageText);
                    
                    // Botones de Game Over
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
