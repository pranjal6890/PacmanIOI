
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

// arena wali file ko include kiya
#include "arena.cpp"
#include "globals.h"

// pacman_movement wali file ko include kiya
#include "pacman_movement.h"

// pacman_draw wali file ko include kiya (animated chomping + death animation)
#include "pacman_draw.h"

// xp_orbs wali file ko include kiya (orbs + score)
#include "xp_orbs.h"

// powerups wali file ko include kiya (powerup animations)
#include "powerups.h"

// sounds wali file ko include kiya (procedural audio)
#include "sounds.h"

// ghost_visuals wali file ko include kiya (ghost rendering)
#include "ghost_visuals.h"

// ghosts logic
#include "ghosts.h"

// victory_screen wali file ko include kiya (end screen UI)
#include "victory_screen.h"

int main() {
  unsigned mapW = (unsigned)baseMap[0].size();
  unsigned mapH = (unsigned)baseMap.size();
  const float uiOffset = 40.0f; // Leaves room at the top for score/lives UI

  // Initialize main rendering window scaled by map tile sizes
  sf::RenderWindow window(sf::VideoMode(sf::Vector2u(
                              mapW * (unsigned)TILE_SIZE,
                              mapH * (unsigned)TILE_SIZE + (unsigned)uiOffset)),
                          "Pac-Man Core");

  // Load font for score display
  sf::Font font("C:/Windows/Fonts/arial.ttf");

  // Create sound buffers and sound objects
  sf::SoundBuffer bBlop = makeBlopSound(), bPow = makePowerupSound(),
                  bShld = makeShieldSound(), bEat = makeGhostEatSound(),
                  bDth = makeDeathSound(), bWin = makeWinSound();
  sf::Sound sBlop(bBlop), sPow(bPow), sShld(bShld), sEat(bEat), sDth(bDth),
      sWin(bWin);

  // game starts
  Entity pacman;
  std::vector<std::shared_ptr<Ghost>> ghosts = initializeGhosts(uiOffset);
  int lives = 3; // Starting lives
  int score = 0;
  bool hasShield = false;
  float shieldTimer = 0.f;
  bool hasPower = false;
  float powerTimer = 0.f;
  // Starting pixel location for Pacman on spawn
  sf::Vector2f pacSpawn(14 * TILE_SIZE + TILE_SIZE / 2,
                        5 * TILE_SIZE + TILE_SIZE / 2 + uiOffset);

  // Spawn orbs on every walkable tile
  std::vector<sf::Vector2f> orbs = spawnOrbs(uiOffset);

  // Spawn powerups on a few random walkable tiles
  auto spawnPowerups = [&]() -> std::vector<Powerup> {
    std::vector<Powerup> pups;
    std::srand((unsigned)std::time(nullptr));
    // Collect walkable positions
    std::vector<sf::Vector2f> spots;
    for (unsigned r = 0; r < baseMap.size(); ++r)
      for (unsigned c = 0; c < baseMap[r].size(); ++c)
        if (baseMap[r][c] != '#' && baseMap[r][c] != 'x' &&
            baseMap[r][c] != '-')
          spots.push_back(
              sf::Vector2f(c * TILE_SIZE + TILE_SIZE / 2,
                           r * TILE_SIZE + TILE_SIZE / 2 + uiOffset));
    // Place one of each type at random positions
    PowerupType types[] = {PowerupType::Health, PowerupType::Power,
                           PowerupType::Shield};
    for (auto t : types) {
      if (spots.empty())
        break;
      int idx = std::rand() % (int)spots.size();
      pups.push_back({spots[idx], t, true});
      spots.erase(spots.begin() + idx);
    }
    return pups;
  };
  std::vector<Powerup> powerups = spawnPowerups();

  auto resetGame = [&]() {
    pacman.pos = pacSpawn;
    pacman.currentDir = Direction::Left;
    pacman.queuedDir = Direction::Left;
    pacman.color = sf::Color::Yellow;
    score = 0;
    lives = 3;
    hasShield = false;
    shieldTimer = 0.f;
    hasPower = false;
    powerTimer = 0.f;
    ghosts = initializeGhosts(uiOffset);
    orbs = spawnOrbs(uiOffset);
    powerups = spawnPowerups();
  };

  // Outer loop: supports Play Again functionality
  bool keepPlaying = true;
  while (keepPlaying) {
    resetGame();
    sf::Clock clock;
    GameState gameState = GameState::Playing;
    float animTime = 0.f;

    // Inner loop: the actual game
    while (window.isOpen() && gameState == GameState::Playing) {
      float dt = clock.restart().asSeconds();
      if (dt > 0.1f)
        dt = 0.1f;
      animTime += dt;

      while (const std::optional<sf::Event> ev = window.pollEvent())
        if (ev->is<sf::Event::Closed>())
          window.close();

      // Take inputs for pacman
      handlePacmanInput(pacman);
      moveEntity(pacman, PACMAN_SPEED, dt, uiOffset, mapW);

      // Update Ghosts
      updateGhosts(ghosts, pacman, dt, uiOffset, animTime);

      bool hitByGhost = checkGhostCollision(pacman, ghosts, hasShield, hasPower,
                                            sEat, sDth, score);
      if (hitByGhost) {
        --lives;
        if (lives <= 0) {
          gameState = GameState::GameOver;
          break;
        }
        pacman.pos = pacSpawn;
      }

      // Collect orbs that Pac-Man touches and play blop sound
      size_t orbsBefore = orbs.size();
      collectOrbs(orbs, pacman.pos, score);
      if (orbs.size() < orbsBefore &&
          sBlop.getStatus() != sf::Sound::Status::Playing)
        sBlop.play();

      // Win condition: all orbs collected
      if (orbs.empty()) {
        gameState = GameState::GameWon;
        sWin.play();
        break;
      }

      // Powerup pickup logic
      for (auto &pu : powerups) {
        if (!pu.active)
          continue;
        if (calcDist(pacman.pos, pu.pos) < TILE_SIZE * 0.7f) {
          pu.active = false;
          switch (pu.type) {
          case PowerupType::Health:
            if (lives < 3)
              ++lives;
            sPow.play();
            break;
          case PowerupType::Power:
            hasPower = true;
            powerTimer = 8.0f;
            frightenGhosts(ghosts, 8.0f);
            sPow.play();
            break;
          case PowerupType::Shield:
            hasShield = true;
            shieldTimer = 10.0f; // 10 sec shield visual
            sShld.play();
            break;
          }
        }
      }

      // Tick powerup timers
      if (hasPower) {
        powerTimer -= dt;
        if (powerTimer <= 0.f) {
          hasPower = false;
        }
      }
      if (hasShield) {
        shieldTimer -= dt;
        if (shieldTimer <= 0.f)
          hasShield = false;
      }

      window.clear(sf::Color::Black);

      drawArena(window, uiOffset, animTime);

      // Draw orbs
      drawOrbs(window, orbs);

      // Draw powerups
      for (auto &pu : powerups) {
        if (!pu.active)
          continue;
        switch (pu.type) {
        case PowerupType::Health:
          drawHealthPowerup(window, pu.pos, animTime);
          break;
        case PowerupType::Power:
          drawPowerPowerup(window, pu.pos, animTime);
          break;
        case PowerupType::Shield:
          drawShieldPowerup(window, pu.pos, animTime);
          break;
        }
      }

      // Animated chomping Pac-Man draw
      drawPacman(window, pacman.pos, pacman.color, pacman.currentDir, animTime);

      // Draw ghosts
      drawGhostsWrapper(window, ghosts, animTime);

      // Draw shield aura around Pac-Man if active
      if (hasShield)
        drawShieldAura(window, pacman.pos, animTime);

      // Draw score and lives HUD
      drawScore(window, font, score, lives);

      window.display();
    }

    // Show victory / game-over screen if window is still open
    if (window.isOpen()) {
      bool won = (gameState == GameState::GameWon);
      EndScreen endScreen(font, won);
      EndChoice choice = endScreen.run(window);

      if (choice == EndChoice::PlayAgain) {
        // Re-open the window if it was closed by the end screen
        if (!window.isOpen()) {
          window.create(sf::VideoMode(sf::Vector2u(mapW * (unsigned)TILE_SIZE,
                                                   mapH * (unsigned)TILE_SIZE +
                                                       (unsigned)uiOffset)),
                        "Pac-Man Core");
        }
        continue; // restart the outer loop
      } else {
        keepPlaying = false;
      }
    } else {
      keepPlaying = false;
    }
  }
  return 0;
}