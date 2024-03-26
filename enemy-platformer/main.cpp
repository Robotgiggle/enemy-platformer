/**
* Author: Ben Miller
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "WalkerEntity.h"
#include "CrawlerEntity.h"
#include "FlyerEntity.h"

// ————— STRUCTS AND ENUMS —————//
struct GameState
{
    Entity* background;
    Entity* player;
    WalkerEntity* walker;
    CrawlerEntity* crawlers;
    FlyerEntity* flyers;
    Entity* platforms;
};

// ————— CONSTANTS ————— //

// window size
const int WINDOW_WIDTH = 640,
          WINDOW_HEIGHT = 480;

// background color
const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

// viewport position & size
const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// shader filepaths
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

// sprite filepaths
const char BACKGROUND_FILEPATH[] = "assets/default_background.png",
           PLAYER_FILEPATH[] = "assets/player.png",
           WALKER_FILEPATH[] = "assets/walker.png",
           CRAWLER_FILEPATH[] = "assets/crawler.png",
           FLYER_FILEPATH[] = "assets/flyer.png",
           PLATFORM_FILEPATH[] = "assets/default_platform.png";

// world constants
const float MILLISECONDS_IN_SECOND = 1000.0;
const float FIXED_TIMESTEP = 0.0166666f;
const float ACC_OF_GRAVITY = -4.91f;

// platforms
const int PLATFORM_COUNT = 13;
const glm::vec3 PLATFORM_COORDS[] = {
    // bottom floor
    glm::vec3(-2.5f,-3.0f,0.0f),
    glm::vec3(-1.5f,-3.0f,0.0f),
    glm::vec3(-0.5f,-3.0f,0.0f),
    glm::vec3(0.5f,-3.0f,0.0f),
    glm::vec3(1.5f,-3.0f,0.0f),
    glm::vec3(2.5f,-3.0f,0.0f),
    // side platforms
    glm::vec3(-4.5f,-1.25f,0.0f),
    glm::vec3(4.5f,-1.25f,0.0f),
    // top floor
    glm::vec3(-2.0f,0.5f,0.0f),
    glm::vec3(-1.0f,0.5f,0.0f),
    glm::vec3(0.0f,0.5f,0.0f),
    glm::vec3(1.0f,0.5f,0.0f),
    glm::vec3(2.0f,0.5f,0.0f),
};

// texture generation stuff
const int NUMBER_OF_TEXTURES = 1;  // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;  // this value MUST be zero

// ————— VARIABLES ————— //

// game state container
GameState g_gameState;

// core globals
SDL_Window* g_displayWindow;
ShaderProgram g_shaderProgram;
glm::mat4 g_viewMatrix, g_projectionMatrix;
bool g_gameIsRunning = true;

// times
float g_previousTicks = 0.0f;
float g_timeAccumulator = 0.0f;

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_displayWindow = SDL_CreateWindow("Platform Fighter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_displayWindow);
    SDL_GL_MakeCurrent(g_displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shaderProgram.load(V_SHADER_PATH, F_SHADER_PATH);

    g_viewMatrix = glm::mat4(1.0f);
    g_projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shaderProgram.set_projection_matrix(g_projectionMatrix);
    g_shaderProgram.set_view_matrix(g_viewMatrix);

    glUseProgram(g_shaderProgram.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— BACKGROUND ————— //
    g_gameState.background = new Entity();
    g_gameState.background->m_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_gameState.background->set_position(glm::vec3(0.0f));
    g_gameState.background->set_width(10.0f);
    g_gameState.background->set_height(7.5f);
    g_gameState.background->update(0.0f, NULL, 0);

    // ————— PLAYER ————— //
    // setup basic attributes
    g_gameState.player = new Entity();
    g_gameState.player->set_motion_type(Entity::SIDE_ON);
    g_gameState.player->set_position(glm::vec3(-2.0f,-2.0f,0.0f));
    g_gameState.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f));
    g_gameState.player->set_speed(1.75f);
    g_gameState.player->set_width(0.65f);
    g_gameState.player->set_height(0.8f);
    g_gameState.player->m_texture_id = load_texture(PLAYER_FILEPATH);
    g_gameState.player->m_jumping_power = 4.5f;

    // setup walking animation
    g_gameState.player->m_walking[Entity::LEFT]  = new int[2] { 0, 2 };
    g_gameState.player->m_walking[Entity::RIGHT] = new int[2] { 1, 3 };
    g_gameState.player->m_animation_indices = g_gameState.player->m_walking[Entity::RIGHT];
    g_gameState.player->setup_anim(2, 2, 2, 6);

    // ————— WALKER ————— //
    g_gameState.walker = new WalkerEntity(false);
    g_gameState.walker->set_position(glm::vec3(2.5f, -2.05f, 0.0f));
    g_gameState.walker->set_speed(2.0f);
    g_gameState.walker->set_width(0.765f);
    g_gameState.walker->set_height(0.9f);
    g_gameState.walker->m_texture_id = load_texture(WALKER_FILEPATH);

    // setup walking animation
    g_gameState.walker->m_walking[Entity::LEFT] = new int[4] { 0, 2 };
    g_gameState.walker->m_walking[Entity::RIGHT] = new int[4] { 1, 3 };
    g_gameState.walker->m_animation_indices = g_gameState.walker->m_walking[Entity::LEFT];
    g_gameState.walker->setup_anim(2, 2, 2, 5);

    // ————— CRAWLER ————— //
    g_gameState.crawlers = new CrawlerEntity[2]{ {0,true}, {2,true} };
    for (int i = 0; i < 2; i++) {
        g_gameState.crawlers[i].set_collision(false);
        g_gameState.crawlers[i].set_position(glm::vec3(-2.0f + 4*i, 1.4f - 1.8*i, 0.0f));
        g_gameState.crawlers[i].set_speed(3.0f);
        g_gameState.crawlers[i].set_width(0.7f);
        g_gameState.crawlers[i].set_height(0.8f);
        g_gameState.crawlers[i].m_texture_id = load_texture(CRAWLER_FILEPATH);

        // setup walking animation
        g_gameState.crawlers[i].m_walking[Entity::LEFT] = new int[4] { 0, 2 };
        g_gameState.crawlers[i].m_walking[Entity::RIGHT] = new int[4] { 1, 3 };
        g_gameState.crawlers[i].m_animation_indices = g_gameState.crawlers[i].m_walking[g_gameState.crawlers[i].get_clockwise()];
        g_gameState.crawlers[i].setup_anim(2, 2, 2, 6);
    }

    // ————— FLYERS ————— //
    g_gameState.flyers = new FlyerEntity[2]{ {0.4f, 0.6f, 3.5f}, {0.4f, 0.6f, 3.5f} };
    for (int i = 0; i < 2; i++) {
        g_gameState.flyers[i].set_position(glm::vec3(8*i - 4.0f, 2.5f, 0.0f));
        // motion type???
        g_gameState.flyers[i].set_speed(4.5f);
        g_gameState.flyers[i].set_width(0.84f);
        g_gameState.flyers[i].set_height(0.63f);
        g_gameState.flyers[i].m_texture_id = load_texture(FLYER_FILEPATH);

        // setup flapping animation
        g_gameState.flyers[i].m_walking[Entity::LEFT] = new int[4] { 0, 2, 4 };
        g_gameState.flyers[i].m_walking[Entity::RIGHT] = new int[4] { 1, 3, 5 };
        g_gameState.flyers[i].m_animation_indices = g_gameState.flyers[i].m_walking[!i];
        g_gameState.flyers[i].setup_anim(2, 3, 3, 4, true);
    }

    // ————— PLATFORMS ————— //
    g_gameState.platforms = new Entity[PLATFORM_COUNT];

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        g_gameState.platforms[i].m_texture_id = load_texture(PLATFORM_FILEPATH);
        g_gameState.platforms[i].set_position(PLATFORM_COORDS[i]);
        g_gameState.platforms[i].update(0.0f, NULL, 0);
    }

    // ————— GENERAL ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // reset forced-movement if no player input
    g_gameState.player->set_movement(glm::vec3(0.0f));
    g_gameState.player->set_rotation(0.0f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

            case SDLK_SPACE:
                if (g_gameState.player->m_collided_bottom) g_gameState.player->m_is_jumping = true;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_gameState.player->move_left();
        g_gameState.player->m_animation_indices = g_gameState.player->m_walking[g_gameState.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_gameState.player->move_right();
        g_gameState.player->m_animation_indices = g_gameState.player->m_walking[g_gameState.player->RIGHT];
    }
    if (key_state[SDL_SCANCODE_Q])
    {
        g_gameState.player->rotate_anticlockwise();
        g_gameState.player->m_animation_indices = g_gameState.player->m_walking[g_gameState.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_E])
    {
        g_gameState.player->rotate_clockwise();
        g_gameState.player->m_animation_indices = g_gameState.player->m_walking[g_gameState.player->RIGHT];
    }

    // normalize forced-movement vector so you don't go faster diagonally
    if (glm::length(g_gameState.player->get_movement()) > 1.0f)
    {
        g_gameState.player->set_movement(glm::normalize(g_gameState.player->get_movement()));
    }
}

void update()
{
    // ————— DELTA TIME ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previousTicks; // the delta time is the difference from the last frame
    g_previousTicks = ticks;

    // ————— FIXED TIMESTEP ————— //
    g_timeAccumulator += delta_time;
    if (g_timeAccumulator < FIXED_TIMESTEP) return;
    while (g_timeAccumulator >= FIXED_TIMESTEP)
    {
        g_gameState.player->update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT);
        g_gameState.walker->update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT);
        g_gameState.crawlers[0].update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT);
        g_gameState.crawlers[1].update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT);
        g_gameState.flyers[0].update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT, g_gameState.player);
        g_gameState.flyers[1].update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT, g_gameState.player);
        g_timeAccumulator -= FIXED_TIMESTEP;
    }
}

void render()
{
    // ————— GENERAL ————— //
    glClear(GL_COLOR_BUFFER_BIT);

    // ————— BACKGROUND ————— //
    g_gameState.background->render(&g_shaderProgram);

    // ————— PLAYER ————— //
    g_gameState.player->render(&g_shaderProgram);

    // ————— ENEMIES ————— //
    g_gameState.walker->render(&g_shaderProgram);
    g_gameState.crawlers[0].render(&g_shaderProgram);
    g_gameState.crawlers[1].render(&g_shaderProgram);
    g_gameState.flyers[0].render(&g_shaderProgram);
    g_gameState.flyers[1].render(&g_shaderProgram);

    // ————— PLATFORM ————— //
    for (int i = 0; i < PLATFORM_COUNT; i++) g_gameState.platforms[i].render(&g_shaderProgram);

    // ————— GENERAL ————— //
    SDL_GL_SwapWindow(g_displayWindow);
}

void shutdown() { 
    SDL_Quit();
    delete[] g_gameState.background;
    delete[] g_gameState.player;
    delete[] g_gameState.walker;
    delete[] g_gameState.crawlers;
    delete[] g_gameState.flyers;
    delete[] g_gameState.platforms;
}

// ————— DRIVER GAME LOOP ————— /
int main(int argc, char* argv[])
{
    initialise();

    while (g_gameIsRunning)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}