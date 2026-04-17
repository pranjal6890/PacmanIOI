#pragma once
#include "ghosts_com.h"

class TrackerGhost : public Ghost {
public:
  TrackerGhost(sf::Color ghostColor) : Ghost(ghostColor) {}

protected:
  sf::Vector2f getSpecificTargetTile(sf::Vector2f pacmanWorldPos,
                                     sf::Vector2f pacmanTilePos,
                                     Direction pacmanDir,
                                     sf::Vector2f scatterTile,
                                     float tileSize) override {
    return pacmanTilePos;
  }
};

class WandererGhost : public Ghost {
public:
  WandererGhost(sf::Color ghostColor) : Ghost(ghostColor) {}

protected:
  sf::Vector2f getSpecificTargetTile(sf::Vector2f pacmanWorldPos,
                                     sf::Vector2f pacmanTilePos,
                                     Direction pacmanDir,
                                     sf::Vector2f scatterTile,
                                     float tileSize) override {
    float distanceToPacman = calcDist(this->pos, pacmanWorldPos);
    float thresholdDistance = tileSize * 8.0f;

    if (distanceToPacman > thresholdDistance) {
      return pacmanTilePos;
    } else {
      return scatterTile;
    }
  }
};

class PredictorGhost : public Ghost {
public:
  PredictorGhost(sf::Color ghostColor) : Ghost(ghostColor) { ghostID = 1; }

protected:
  sf::Vector2f getSpecificTargetTile(sf::Vector2f pacmanWorldPos,
                                     sf::Vector2f pacmanTilePos,
                                     Direction pacmanDir,
                                     sf::Vector2f scatterTile,
                                     float tileSize) override {
    sf::Vector2f movementDir = getDirectionVector(pacmanDir);
    sf::Vector2f targetPos;

    targetPos.x = pacmanTilePos.x + (movementDir.x * 4);
    targetPos.y = pacmanTilePos.y + (movementDir.y * 4);

    return targetPos;
  }
};

class AmbusherGhost : public Ghost {
public:
  AmbusherGhost(sf::Color ghostColor) : Ghost(ghostColor) { ghostID = 3; }

  float getSpeedMultiplier() const override { return 1.2f; }

protected:
  sf::Vector2f getSpecificTargetTile(sf::Vector2f pacmanWorldPos,
                                     sf::Vector2f pacmanTilePos,
                                     Direction pacmanDir,
                                     sf::Vector2f scatterTile,
                                     float tileSize) override {
    sf::Vector2f movementDir = getDirectionVector(pacmanDir);
    sf::Vector2f targetPos;

    targetPos.x = pacmanTilePos.x - (movementDir.x * 2);
    targetPos.y = pacmanTilePos.y - (movementDir.y * 2);

    return targetPos;
  }
};

// ─────────────────────────────────────────────
//  Ghost Initialization (using behavior-driven types)
// ─────────────────────────────────────────────
inline std::vector<std::shared_ptr<Ghost>> initializeGhosts(float uiOffset) {
  std::vector<std::shared_ptr<Ghost>> ghosts;

  sf::Vector2f spawnPoints[] = {
      sf::Vector2f(12 * TILE_SIZE + TILE_SIZE / 2,
                   13 * TILE_SIZE + TILE_SIZE / 2 + uiOffset),
      sf::Vector2f(13 * TILE_SIZE + TILE_SIZE / 2,
                   13 * TILE_SIZE + TILE_SIZE / 2 + uiOffset),
      sf::Vector2f(14 * TILE_SIZE + TILE_SIZE / 2,
                   13 * TILE_SIZE + TILE_SIZE / 2 + uiOffset),
      sf::Vector2f(15 * TILE_SIZE + TILE_SIZE / 2,
                   13 * TILE_SIZE + TILE_SIZE / 2 + uiOffset)};

  // TrackerGhost  — Red,    chases Pac-Man directly
  auto tracker = std::make_shared<TrackerGhost>(sf::Color::Red);
  tracker->ghostID = 0;
  ghosts.push_back(tracker);

  // PredictorGhost — Pink,  targets 4 tiles ahead of Pac-Man
  auto predictor = std::make_shared<PredictorGhost>(sf::Color(255, 184, 255));
  predictor->ghostID = 1;
  ghosts.push_back(predictor);

  // WandererGhost — Cyan,   chases if far, scatters if close
  auto wanderer = std::make_shared<WandererGhost>(sf::Color::Cyan);
  wanderer->ghostID = 2;
  ghosts.push_back(wanderer);

  // AmbusherGhost — Orange, faster, targets behind Pac-Man
  auto ambusher = std::make_shared<AmbusherGhost>(sf::Color(255, 184, 82));
  ambusher->ghostID = 3;
  ghosts.push_back(ambusher);

  for (size_t i = 0; i < ghosts.size(); i++) {
    ghosts[i]->pos = spawnPoints[i];
    ghosts[i]->currentDir = Direction::Left;
  }

  return ghosts;
}