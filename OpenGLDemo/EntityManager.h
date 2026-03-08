#pragma once
#include "Enemy.h"
#include "Player.h"
#include "InputHandler.h"
#include "Map.h"
#include <vector>
#include <memory>
#include <cstdlib>

//Spawns and stores all characters including enemies, player, missiles and powerups. Also owns the Map and InputHandler.
class EntityManager
{
public:
    //Input updated inside PhysicsManager but need to activate it here to provide player reference
    InputHandler input;

    EntityManager(b2WorldId worldId, Renderer* renderer, int width = 1200, int height = 800)
        : worldId(worldId), renderer(renderer), width(width), height(height)
    {
        AwakeCharacters();
        input.ActivateInput(player);
    }

    //All alive characters — enemies, missiles, powerups, and player
    std::vector<std::unique_ptr<Character>> characters;

    //Called once on construction player and map
    void AwakeCharacters();

    //Checks Spawn → out-of-bounds KILL → add missiles from player loner and asteroids  →despawn 
    void CharacterUpdate(float deltaTime);

    //Map + all characters rendered in correct order
    void Render(float deltaTime);

    //Overwrite the map
    void LoadMap(const std::string& bgTexPath, float bgScrollSpeed,
        const std::string& fgTexPath, float fgScrollSpeed)
    {
        map.Cleanup();
        map.Init(*renderer, bgTexPath, bgScrollSpeed, fgTexPath, fgScrollSpeed,
            (float)width, (float)height);
    }

    //Used by PhysicsManager and FrameworkManager
    std::vector<std::unique_ptr<Character>>& getCharacters() { return characters; }
    Player* getPlayer()  const { return player; }
    Map& GetMap() { return map; }

    //Drone wave state used to spawn drone enemies which spawn in waves
    int       dronesLeft = 0;
    glm::vec2 waveSpawnPos = { 0.0f, 0.0f };

private:
    b2WorldId  worldId;
    Renderer* renderer = nullptr;
    int        width;
    int        height;

    Map    map;
    Player* player = nullptr;

    //Enemy spawn timer
    float spawnTimer = 0.0f;
    float spawnRate = 1.0f;

    //Spawn random enemy or weapon powerUp can spawn asteroids any size
    void SpawnRandom();

    //Drain children spawned inside other characters
    void DrainChildSpawns();
};