
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "arena.cpp"
#include "globals.h"

#include "pacman_movement.h"

#include "pacman_draw.h"

#include "xp_orbs.h"

#include "powerups.h"

#include "sounds.h"

#include "victory_screen.h"

std::vector<Powerup> spawnPowerups(float uiOffset) {
  std::vector<Powerup> pups;
  std::srand((unsigned)std::time(nullptr));
  // Collect walkable positions
  std::vector<sf::Vector2f> spots;
  for (unsigned r = 0; r < baseMap.size(); r++) {
    for (unsigned c = 0; c < baseMap[r].size(); c++) {
      if (baseMap[r][c] != '#' && baseMap[r][c] != 'x' && baseMap[r][c] != '-') {
        spots.push_back(sf::Vector2f(c * TILE_SIZE + TILE_SIZE / 2,
                                     r * TILE_SIZE + TILE_SIZE / 2 + uiOffset));
      }
    }
  }
  // Place one of each type at random positions
  PowerupType types[] = {PowerupType::Health, PowerupType::Power, PowerupType::Shield};
  for (auto t : types) {
    if (spots.empty()) break;
    int idx = std::rand() % (int)spots.size();
    pups.push_back({spots[idx], t, true});
    spots.erase(spots.begin() + idx);
  }
  return pups;
}

int main() {
  unsigned mapW = (unsigned)baseMap[0].size();
  unsigned mapH = (unsigned)baseMap.size();
  const float uiOffset = 40.0f; 

  sf::RenderWindow window(sf::VideoMode(sf::Vector2u(
                              mapW * (unsigned)TILE_SIZE,
                              mapH * (unsigned)TILE_SIZE + (unsigned)uiOffset)),
                          "Pac-Man Core");

  sf::Font font("C:/Windows/Fonts/arial.ttf");

  sf::SoundBuffer bBlop = makeBlopSound(), bPow = makePowerupSound(),
                  bShld = makeShieldSound(), bEat = makeGhostEatSound(),
                  bDth = makeDeathSound(), bWin = makeWinSound();
  sf::Sound sBlop(bBlop), sPow(bPow), sShld(bShld), sEat(bEat), sDth(bDth), sWin(bWin);

  Entity pacman;
  int score = 0;
  bool hasShield = false;
  float shieldTimer = 0.f;
  bool hasPower = false;
  float powerTimer = 0.f;
  sf::Vector2f pacSpawn(14 * TILE_SIZE + TILE_SIZE / 2,
                        5 * TILE_SIZE + TILE_SIZE / 2 + uiOffset);

  std::vector<sf::Vector2f> orbs = spawnOrbs(uiOffset);
  std::vector<Powerup> powerups = spawnPowerups(uiOffset);

  bool keepPlaying = true;

  while (keepPlaying) {
    // Reset game state
    pacman.pos = pacSpawn;
    pacman.currentDir = Direction::Left;
    pacman.queuedDir = Direction::Left;
    pacman.color = sf::Color::Yellow;
    score = 0;
    hasShield = false;
    shieldTimer = 0.f;
    hasPower = false;
    powerTimer = 0.f;
    orbs = spawnOrbs(uiOffset);
    powerups = spawnPowerups(uiOffset);

    sf::Clock clock;
    GameState gameState = GameState::Playing;
    float animTime = 0.f;

    while (window.isOpen() && gameState == GameState::Playing) {
      float dt = clock.restart().asSeconds();
      if (dt > 0.1f) dt = 0.1f;
      animTime += dt;

      while (const std::optional<sf::Event> ev = window.pollEvent())
        if (ev->is<sf::Event::Closed>())
          window.close();

      handlePacmanInput(pacman);
      moveEntity(pacman, PACMAN_SPEED, dt, uiOffset, mapW);

      size_t orbsBefore = orbs.size();
      collectOrbs(orbs, pacman.pos, score);
      if (orbs.size() < orbsBefore && sBlop.getStatus() != sf::Sound::Status::Playing)
        sBlop.play();

      for (auto &pu : powerups) {
        if (!pu.active) continue;
        if (calcDist(pacman.pos, pu.pos) < TILE_SIZE * 0.7f) {
          pu.active = false;
          if (pu.type == PowerupType::Health) {
            score += 50;  
            sPow.play();
          }
          else if (pu.type == PowerupType::Power) {
            hasPower = true;
            powerTimer = 8.0f;  
            sPow.play();
          }
          else if (pu.type == PowerupType::Shield) {
            hasShield = true;
            shieldTimer = 10.0f;  
            sShld.play();
          }
        }
      }

      if (hasPower) { powerTimer -= dt; if (powerTimer <= 0.f) hasPower = false; }
      if (hasShield) { shieldTimer -= dt; if (shieldTimer <= 0.f) hasShield = false; }

      // Check win condition
      if (orbs.empty()) {
        gameState = GameState::GameWon;
        sWin.play();
      }

      window.clear(sf::Color::Black);

      drawArena(window, uiOffset, animTime);

      drawOrbs(window, orbs);

      for (auto &pu : powerups) {
        if (!pu.active) continue;
        switch (pu.type) {
          case PowerupType::Health: drawHealthPowerup(window, pu.pos, animTime); break;
          case PowerupType::Power:  drawPowerPowerup(window, pu.pos, animTime);  break;
          case PowerupType::Shield: drawShieldPowerup(window, pu.pos, animTime); break;
        }
      }

      drawPacman(window, pacman.pos, pacman.color, pacman.currentDir, animTime);

      if (hasShield)
        drawShieldAura(window, pacman.pos, animTime);

      drawScore(window, font, score);

      window.display();
    }

    if (window.isOpen()) {
      bool won = (gameState == GameState::GameWon);
      EndScreen endScreen(font, won);
      EndChoice choice = endScreen.run(window);

      if (choice == EndChoice::PlayAgain) {
        if (!window.isOpen()) {
          window.create(sf::VideoMode(sf::Vector2u(
                            mapW * (unsigned)TILE_SIZE,
                            mapH * (unsigned)TILE_SIZE + (unsigned)uiOffset)),
                        "Pac-Man Core");
        }
        continue;  
      }
      else {
        keepPlaying = false;
      }
    }
    else {
      keepPlaying = false;
    }
  }

  return 0;
}
