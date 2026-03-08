#include "FrameworkManager.h"
#include <iostream>
#include <ctime>

bool FrameworkManager::Init(int width, int height)
{
    m_width = width;
    m_height = height;

    // ── ──────────────────────────────────────────────────────────────── SDL ────────────────────────────────────────────────────────────────
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    //  ────────────────────────────────────────────────────────────────── OpenGL context attributes — must be set before window creation ────────────────────────────────────────
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_window = SDL_CreateWindow("XenonGame", width, height, SDL_WINDOW_OPENGL);
    if (!m_window) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::cout << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    // ──────────────────────────────GLAD───────────────────────────────────
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DestroyContext(m_glContext);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    // ─────────────────────────OpenGL state────────────────────────────────
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);

    // ───────────────────────── Renderer────────────────────────────────────
    m_renderer.Init("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    //Projection made to match the box2d
    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    m_renderer.SetProjection(projection);

    // ──────────────────────────── Box2D ────────────────────────────────────
    std::srand((unsigned)std::time(nullptr));
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0.0f, 0.0f };
    m_worldId = b2CreateWorld(&worldDef);

    //If order is wrong script breaks
    m_entities = new EntityManager(m_worldId, &m_renderer, width, height);

    //PhysicsManager needs entityManager ready since it needs characters
    m_physics = new PhysicsManager(m_worldId, *m_entities);

    m_running = true;
    return true;
}

void FrameworkManager::Run()
{
    m_previousTime = (float)SDL_GetTicks() / 1000.0f;

    while (m_running)
    {
        Uint64 ticks = SDL_GetTicks();
        float  time = ticks / 1000.0f;
        float  deltaTime = time - m_previousTime;

        //Prevents issues with really low fps
        if (deltaTime > 0.25f) deltaTime = 0.25f;
        m_previousTime = time;

        //Checks if tgere uinst a quit event
        SDL_Event event;
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) m_running = false;

        //Check for player death ends program on player dying
        if (m_entities->getPlayer() && m_entities->getPlayer()->IsDead()) {
            std::cout << "Player has died — game over" << std::endl;
            m_running = false;
        }

        Update(deltaTime);
        Render(deltaTime);

        // FPS cap
        Uint64 frameTime = SDL_GetTicks() - ticks;
        if (frameTime < TARGET_FRAMETIME)
            SDL_Delay((Uint32)(TARGET_FRAMETIME - frameTime));
    }
}

void FrameworkManager::Update(float deltaTime)
{
    //PlayerInput → physics step → position sync → checks contacts
    m_physics->UpdatePhysics(deltaTime);

    //Checks Spawn → out-of-bounds KILL → add missiles from player loner and asteroids  →despawn 
    m_entities->CharacterUpdate(deltaTime);
}

void FrameworkManager::Render(float deltaTime)
{
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Map + all characters + player rendered inside EntityManager
    m_entities->Render(deltaTime);

    SDL_GL_SwapWindow(m_window);
}

void FrameworkManager::Shutdown()
{
    //Deletion order matters if I am not wrong

    //Entities first — destroys b2Bodies while world still exists
    delete m_physics;
    delete m_entities;

    //World after bodies are gone
    b2DestroyWorld(m_worldId);

    m_renderer.Cleanup();
    SDL_GL_DestroyContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}