#include "EntityManager.h"
#include "Rusher.h"
#include "RockAsteroid.h"
#include "MetalAsteroid.h"
#include "Loner.h"
#include "Drone.h"
#include "EnemyMissile.h"
#include "WeaponPowerUp.h"
#include "LifePowerUp.h"


void EntityManager::AwakeCharacters()
{
    auto playerPtr = std::make_unique<Player>(
        worldId,
        glm::vec2(width * 0.5f, height * 0.5f),
        glm::vec2(50.0f, 50.0f),
        270.0f,    // rotation
        1000.0f,   // damage
        (float)width, (float)height);

    player = playerPtr.get();
    characters.push_back(std::move(playerPtr));

    map.Init(*renderer,
        "XenonSprites/Background1.bmp", 80.0f,
        "XenonSprites/Background2.bmp", 200.0f,
        (float)width, (float)height);
}

void EntityManager::CharacterUpdate(float deltaTime)
{
    map.Update(deltaTime);

    // ─────────────────────Enemy spawn timer───────────────────────────────
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnRate) {
        spawnTimer = 0.0f;
        SpawnRandom();
    }

    //Drones spawn one at a time after the wave is triggered in SpawnRandom
    if (dronesLeft > 0 && spawnTimer >= 0.4f) {
        spawnTimer = 0.0f;
        dronesLeft--;
        characters.push_back(std::make_unique<Drone>(worldId, waveSpawnPos));
    }

    //Drain child spawns from enemies player is done seperatly(asteroids, loner missiles)
    DrainChildSpawns();

    for (auto& c : characters)
    {
        //Checks if character left the map bounds if they did they Die
        if (c->position.x <= -10 || c->position.x >= width + 100 || c->position.y <= -10 || c->position.y >= width + 100)
        {
            //Forces any chararacter to die
            c->isDead = true;
        }
    }

    //Not the best implementation I know
    if (player) {
        for (auto& missile : player->missiles) {
            missile->UpdatePosition(); // sync position on spawn frame
            characters.push_back(std::move(missile));
        }
        player->missiles.clear();
    }

    //Skip player when erasing dead characters
    //Remove all dead characters, except player
    characters.erase(std::remove_if(characters.begin(), characters.end(), [this](const std::unique_ptr<Character>& c) {return c->IsDead() && c.get() != player; }), characters.end());
}


void EntityManager::Render(float deltaTime)
{
    //Map drawn first
    map.Render(*renderer, deltaTime);


    //Render every character
    for (auto& c : characters)
        c->Render(*renderer, deltaTime);
}


void EntityManager::DrainChildSpawns()
{
    std::vector<std::unique_ptr<Character>> toAdd;

    for (auto& c : characters) {
        //RockAsteroid splits into smaller asteroids on death
        RockAsteroid* asteroid = dynamic_cast<RockAsteroid*>(c.get());
        if (asteroid && !asteroid->spawnedAsteroids.empty()) {
            for (auto& spawned : asteroid->spawnedAsteroids)
                toAdd.push_back(std::move(spawned));
            asteroid->spawnedAsteroids.clear();
        }

        //Loner fires missiles at the player
        Loner* loner = dynamic_cast<Loner*>(c.get());
        if (loner && !loner->spawnedMissiles.empty()) {
            for (auto& spawned : loner->spawnedMissiles)
                toAdd.push_back(std::move(spawned));
            loner->spawnedMissiles.clear();
        }
    }

    for (auto& a : toAdd)
        characters.push_back(std::move(a));
}



void EntityManager::SpawnRandom()
{
    int spawnX = width;
    int spawnY = height;

    int randomSpawnX = 50.0f + (std::rand() % (int)width);
    int randomSpawnY = (std::rand() % (int)height) - 50.0f;

    int roll = std::rand() % 10;

    std::cout << "SpawnRandom roll: " << roll << std::endl;

    if (roll == 0) characters.push_back(std::make_unique<Rusher>(worldId, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 1) characters.push_back(std::make_unique<RockAsteroid>(worldId, AsteroidSize::Large, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 2) characters.push_back(std::make_unique<RockAsteroid>(worldId, AsteroidSize::Medium, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 3) characters.push_back(std::make_unique<MetalAsteroid>(worldId, AsteroidSize::Large, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 4) characters.push_back(std::make_unique<MetalAsteroid>(worldId, AsteroidSize::Medium, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 5) characters.push_back(std::make_unique<MetalAsteroid>(worldId, AsteroidSize::Small, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 6) characters.push_back(std::make_unique<Loner>(player, worldId, glm::vec2(randomSpawnX, spawnY))); //Only one who spawns from top
    else if (roll == 7) {
        //Drone spawn inside update
        dronesLeft = 8;
        spawnTimer = 0.0f;
        waveSpawnPos = glm::vec2(spawnX, randomSpawnY);
    }
    else if (roll == 8) characters.push_back(std::make_unique<WeaponPowerUp>(player, worldId, glm::vec2(spawnX, randomSpawnY)));
    else if (roll == 9) characters.push_back(std::make_unique<LifePowerUp>(player, worldId, glm::vec2(spawnX, randomSpawnY)));
}