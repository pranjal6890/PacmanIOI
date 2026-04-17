// ghosts_com.h
#pragma once

#include "ghost_direction.h"
#include "ghost_visuals.h"
#include "pacman_movement.h"
#include <iostream>
#include <memory>


// ─────────────────────────────────────────────
//  Confused Star Orbit Effect
// ─────────────────────────────────────────────
inline void drawConfusedStars(sf::RenderWindow &window, sf::Vector2f ghostPos,
                              float animTime) {
  sf::Vector2f center = ghostPos + sf::Vector2f(0, -15.0f);
  float orbitR = 8.0f, sr = 2.8f;
  for (int i = 0; i < 3; ++i) {
    float angle = animTime * 5.0f + i * (2 * 3.14159f / 3.0f);
    sf::Vector2f sp = center + sf::Vector2f(std::cos(angle) * orbitR,
                                            std::sin(angle) * orbitR * 0.45f);
    sf::VertexArray star(sf::PrimitiveType::Triangles, 24);
    sf::Color sc(255, 230, 0);
    for (int j = 0; j < 8; ++j) {
      float a1 = j * 2 * 3.14159f / 8.0f;
      float a2 = (j + 1) * 2 * 3.14159f / 8.0f;
      float r1 = (j % 2 == 0) ? sr : sr * 0.4f;
      float r2 = ((j + 1) % 2 == 0) ? sr : sr * 0.4f;
      star[j * 3 + 0] = sf::Vertex{sp, sc};
      star[j * 3 + 1] = sf::Vertex{
          sp + sf::Vector2f(std::cos(a1) * r1, std::sin(a1) * r1), sc};
      star[j * 3 + 2] = sf::Vertex{
          sp + sf::Vector2f(std::cos(a2) * r2, std::sin(a2) * r2), sc};
    }
    window.draw(star);
  }
}

// ─────────────────────────────────────────────
//  Abstract Base Ghost Class
// ─────────────────────────────────────────────
class Ghost {
public:
  sf::Vector2f pos;
  Direction currentDir = Direction::None;
  sf::Color color;
  int mood = 0;
  bool confused = false;
  bool respawning = false;
  float confuseTimer = 0.0f;
  float respawnTimer = 0.0f;
  int ghostID = 0;

  explicit Ghost(sf::Color c) : color(c) {}
  virtual ~Ghost() = default;

  sf::Vector2f getFinalTargetTile(int gx, int gy, sf::Vector2f pacWorldPos,
                                  sf::Vector2f pacTilePos, Direction pacDir,
                                  sf::Vector2f scatterTile, float tileSize) {
    if (respawning)
      return {14.f, 11.f};
    if (gy >= 11 && gy <= 15 && gx >= 10 && gx <= 17)
      return {14.f, 9.f};
    return getSpecificTargetTile(pacWorldPos, pacTilePos, pacDir, scatterTile,
                                 tileSize);
  }

  virtual float getSpeedMultiplier() const { return 1.0f; }

protected:
  virtual sf::Vector2f getSpecificTargetTile(sf::Vector2f pacWorldPos,
                                             sf::Vector2f pacTilePos,
                                             Direction pacDir,
                                             sf::Vector2f scatterTile,
                                             float tileSize) = 0;
};

// ─────────────────────────────────────────────
//  Game Logic Helpers
// ─────────────────────────────────────────────

inline sf::Vector2f getScatterTarget(int ghostID) {
  switch (ghostID) {
  case 0:
    return sf::Vector2f(26.f, 1.f);
  case 1:
    return sf::Vector2f(1.f, 1.f);
  case 2:
    return sf::Vector2f(26.f, 27.f);
  case 3:
    return sf::Vector2f(1.f, 27.f);
  default:
    return sf::Vector2f(1.f, 1.f);
  }
}

inline void moveGhost(std::shared_ptr<Ghost> &ghost, float spd, float dt,
                      float uiOffset, unsigned mapW, sf::Vector2f targetTile) {
  int gx = (int)(ghost->pos.x / TILE_SIZE);
  int gy = (int)((ghost->pos.y - uiOffset) / TILE_SIZE);
  sf::Vector2f tc(gx * TILE_SIZE + TILE_SIZE / 2,
                  gy * TILE_SIZE + TILE_SIZE / 2 + uiOffset);
  sf::Vector2f toC = tc - ghost->pos;
  sf::Vector2f cDir = getDirectionVector(ghost->currentDir);
  bool towards = (toC.x * cDir.x + toC.y * cDir.y) > 0;

  if (calcDist(ghost->pos, tc) < 0.1f ||
      (towards && calcDist(ghost->pos, tc) <= spd * dt)) {
    ghost->pos = tc;
    Direction bestDir = Direction::None;
    float bestDist = 999999.0f;
    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left,
                        Direction::Right};
    sf::Vector2f dirVecs[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    Direction reverseDir = Direction::None;
    if (ghost->currentDir == Direction::Up)
      reverseDir = Direction::Down;
    if (ghost->currentDir == Direction::Down)
      reverseDir = Direction::Up;
    if (ghost->currentDir == Direction::Left)
      reverseDir = Direction::Right;
    if (ghost->currentDir == Direction::Right)
      reverseDir = Direction::Left;

    // Forced random direction when confused logic
    bool chosen = false;
    if (ghost->confused && !ghost->respawning) {
      std::vector<Direction> validDirs;
      for (int i = 0; i < 4; i++) {
        if (dirs[i] == reverseDir && ghost->currentDir != Direction::None)
          continue;
        int nextX = gx + (int)dirVecs[i].x;
        int nextY = gy + (int)dirVecs[i].y;
        if (!isWall(nextX, nextY, true))
          validDirs.push_back(dirs[i]);
      }
      if (!validDirs.empty()) {
        bestDir = validDirs[std::rand() % validDirs.size()];
        chosen = true;
      }
    }

    if (!chosen) {
      for (int i = 0; i < 4; i++) {
        if (dirs[i] == reverseDir && ghost->currentDir != Direction::None)
          continue;
        int nextX = gx + (int)dirVecs[i].x;
        int nextY = gy + (int)dirVecs[i].y;
        if (!isWall(nextX, nextY, true)) {
          float newDist =
              std::hypot(targetTile.x - nextX, targetTile.y - nextY);
          if (newDist < bestDist) {
            bestDist = newDist;
            bestDir = dirs[i];
          }
        }
      }
    }

    if (bestDir == Direction::None)
      bestDir = reverseDir;
    ghost->currentDir = bestDir;
    ghost->pos += getDirectionVector(ghost->currentDir) * spd * dt;
  } else {
    ghost->pos += cDir * spd * dt;
  }

  if (ghost->pos.x < 0)
    ghost->pos.x += mapW * TILE_SIZE;
  if (ghost->pos.x >= (float)mapW * TILE_SIZE)
    ghost->pos.x -= mapW * TILE_SIZE;
}

inline void updateGhosts(std::vector<std::shared_ptr<Ghost>> &ghosts,
                         const Entity &pacman, float dt, float uiOffset,
                         float globalTime) {
  bool scatterMode =
      (std::fmod(globalTime, 27.0f) < 7.0f); // 7 sec scatter, 20 sec chase
  sf::Vector2f pacTilePos(pacman.pos.x / TILE_SIZE,
                          (pacman.pos.y - uiOffset) / TILE_SIZE);
  int mapW = (int)baseMap[0].size();

  for (auto &ghost : ghosts) {
    if (ghost->respawning) {
      ghost->respawnTimer -= dt;
      if (ghost->respawnTimer <= 0.f) {
        ghost->respawning = false;
        ghost->confused = false;
      }
    } else if (ghost->confused) {
      ghost->confuseTimer -= dt;
      if (ghost->confuseTimer <= 0.f)
        ghost->confused = false;
    }

    int gx = (int)(ghost->pos.x / TILE_SIZE);
    int gy = (int)((ghost->pos.y - uiOffset) / TILE_SIZE);

    sf::Vector2f targetTile;
    if (scatterMode && !ghost->respawning) {
      targetTile = getScatterTarget(ghost->ghostID);
    } else {
      targetTile = ghost->getFinalTargetTile(
          gx, gy, pacman.pos, pacTilePos, pacman.currentDir,
          getScatterTarget(ghost->ghostID), TILE_SIZE);
    }

    float ghostSpeed = PACMAN_SPEED * 0.85f * ghost->getSpeedMultiplier();
    if (ghost->confused)
      ghostSpeed *= 0.5f;
    if (ghost->respawning)
      ghostSpeed *= 2.0f;

    moveGhost(ghost, ghostSpeed, dt, uiOffset, mapW, targetTile);
  }
}

inline bool checkGhostCollision(Entity &pacman,
                                std::vector<std::shared_ptr<Ghost>> &ghosts,
                                bool hasShield, bool hasPower, sf::Sound &sEat,
                                sf::Sound &sDth, int &score) {
  for (auto &ghost : ghosts) {
    float dist = calcDist(pacman.pos, ghost->pos);
    if (dist < TILE_SIZE * 0.6f) {
      if (ghost->respawning) {
        continue; // Cannot hit a respawning ghost
      }
      if (ghost->confused) {
        sEat.play();
        score += 200;
        ghost->respawning = true;
        ghost->respawnTimer = 5.0f;
        ghost->confused = false;
      } else if (!hasShield) {
        sDth.play();
        return true;
      }
    }
  }
  return false;
}

inline void drawGhostsWrapper(sf::RenderWindow &window,
                              const std::vector<std::shared_ptr<Ghost>> &ghosts,
                              float animTime) {
  for (const auto &ghost : ghosts) {
    if (ghost->respawning) {
      drawGhostFace(window, ghost->pos, ghost->currentDir, 0);
      continue;
    }

    int mood = 0; // 0 normal, 1 angry, 2 frightened
    sf::Color drawCol = ghost->color;

    if (ghost->confused) {
      mood = 2; // Frightened
      drawCol = sf::Color::Blue;
    } else {
      mood = 1; // Angry
    }

    drawGhost(window, ghost->pos, drawCol, ghost->currentDir, mood, animTime,
              TILE_SIZE);

    if (ghost->confused) {
      drawConfusedStars(window, ghost->pos, animTime);
    }
  }
}

inline void frightenGhosts(std::vector<std::shared_ptr<Ghost>> &ghosts,
                           float duration = 8.0f) {
  for (auto &ghost : ghosts) {
    if (!ghost->respawning) {
      ghost->confused = true;
      ghost->confuseTimer = duration;
    }
  }
}
