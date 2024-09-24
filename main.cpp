#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

// Dimensiones de la ventana
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Estructura para representar el Paddle
struct Paddle {
    float x, y, w, h;
};

// Estructura para representar la Pelota
struct Ball {
    float x, y, w, h;
    float velX, velY;
};

// Estructura para representar los Bloques
struct Block {
    float x, y, w, h;
    bool destroyed;  // Bandera para marcar si el bloque ha sido destruido
};

// Variables globales
Paddle paddle = {WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 30, 100, 20};
Ball ball = {WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT / 2 - 10, 20, 20, 0.2f, 0.2f};
bool running = true;
std::vector<Block> blocks;  // Vector para almacenar los bloques

// Función para inicializar los bloques
void initBlocks() {
    int rows = 5;
    int cols = 10;
    float blockWidth = WINDOW_WIDTH / cols;
    float blockHeight = 30;
    
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            Block block = {col * blockWidth, row * blockHeight, blockWidth - 5, blockHeight - 5, false};
            blocks.push_back(block);
        }
    }
}

// Función para manejar eventos
void handleEvents(SDL_Event& event) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}

// Función para actualizar la lógica del juego (incluyendo delta time)
void update(float deltaTime) {
    // Movimiento del Paddle
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) {
        paddle.x -= 400.0f * deltaTime;
        if (paddle.x < 0) paddle.x = 0;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        paddle.x += 400.0f * deltaTime;
        if (paddle.x + paddle.w > WINDOW_WIDTH) paddle.x = WINDOW_WIDTH - paddle.w;
    }

    // Movimiento de la pelota
    ball.x += ball.velX * 400 * deltaTime;
    ball.y += ball.velY * 400 * deltaTime;

    // Colisión con las paredes
    if (ball.x <= 0 || ball.x + ball.w >= WINDOW_WIDTH) {
        ball.velX = -ball.velX;
    }
    if (ball.y <= 0) {
        ball.velY = -ball.velY;
    }

    // Colisión con el paddle
    if (ball.x + ball.w >= paddle.x && ball.x <= paddle.x + paddle.w &&
        ball.y + ball.h >= paddle.y && ball.y <= paddle.y + paddle.h) {
        ball.velY = -ball.velY;
        ball.velX *= 1.1f; // Aumentar la velocidad con cada colisión
    }

    // Colisión con los bloques
    for (Block& block : blocks) {
        if (!block.destroyed && 
            ball.x + ball.w >= block.x && ball.x <= block.x + block.w &&
            ball.y + ball.h >= block.y && ball.y <= block.y + block.h) {
            block.destroyed = true;  // Marca el bloque como destruido
            ball.velY = -ball.velY;  // Invierte la dirección de la pelota
            break;  // Sale del loop después de colisionar con un bloque
        }
    }

    // Game Over
    if (ball.y > WINDOW_HEIGHT) {
        std::cout << "Game Over!" << std::endl;
        std::cout << "Press Enter to close the game." << std::endl;
        std::cin.get();  
        running = false;
    }

    // Win Condition: Si todos los bloques están destruidos
    bool allBlocksDestroyed = true;
    for (const Block& block : blocks) {
        if (!block.destroyed) {
            allBlocksDestroyed = false;
            break;
        }
    }

    if (allBlocksDestroyed) {
        std::cout << "You Win!" << std::endl;
        std::cout << "Press Enter to close the game." << std::endl;
        std::cin.get();  
        running = false;
    }
}

// Renderizado de la escena
void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Dibujar paddle
    SDL_Rect paddleRect = {static_cast<int>(paddle.x), static_cast<int>(paddle.y), static_cast<int>(paddle.w), static_cast<int>(paddle.h)};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddleRect);

    // Dibujar pelota
    SDL_Rect ballRect = {static_cast<int>(ball.x), static_cast<int>(ball.y), static_cast<int>(ball.w), static_cast<int>(ball.h)};
    SDL_RenderFillRect(renderer, &ballRect);

    // Dibujar bloques
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Color rojo para los bloques
    for (const Block& block : blocks) {
        if (!block.destroyed) {
            SDL_Rect blockRect = {static_cast<int>(block.x), static_cast<int>(block.y), static_cast<int>(block.w), static_cast<int>(block.h)};
            SDL_RenderFillRect(renderer, &blockRect);
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error inicializando SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Inicializar bloques
    initBlocks();

    Uint32 lastTime = SDL_GetTicks();
    SDL_Event event;

    // Ciclo principal del juego
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handleEvents(event);
        update(deltaTime);
        render(renderer);
    }

    // Limpiar SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
