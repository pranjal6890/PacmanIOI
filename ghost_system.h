#pragma once

#include "globals.h"
#include <vector>
#include <cmath>
#include <queue>
#include "pacman_movement.h"
#include <SFML/Audio/Sound.hpp>

// Ghost AI modes
enum class GhostMode {
    CHASE,      // Hunt Pacman
    SCATTER,    // Move toward corner
    FRIGHTENED  // Run away / vulnerable
};

struct Ghost {
    Entity entity;
    GhostMode mode;
    sf::Clock modeTimer;
    sf::Clock scatterTimer;
    float scatterDuration;
    float chaseDuration;
    int ghostID;  // 0, 1, 2, 3 etc
};

// ─── Ghost Initialization ───────────────────────────────────────────────────
std::vector<Ghost> initializeGhosts(float uiOffset) {
    std::vector<Ghost> ghosts;
    
    // Ghost spawn positions (in tile coords, middle of ghost house)
    sf::Vector2f spawnPoints[] = {
        sf::Vector2f(12 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset),  // red
        sf::Vector2f(13 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset),  // pink
        sf::Vector2f(14 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset),  // blue
        sf::Vector2f(15 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset)   // orange
    };
    
    sf::Color ghostColors[] = {
        sf::Color::Red,
        sf::Color::Magenta,
        sf::Color::Blue,
        sf::Color::Yellow
    };
    
    for (int i = 0; i < 4; i++) {
        Ghost ghost;
        ghost.entity.pos = spawnPoints[i];
        ghost.entity.color = ghostColors[i];
        ghost.entity.currentDir = Direction::Left;
        ghost.entity.ghostIndex = i;
        ghost.mode = GhostMode::SCATTER;
        ghost.scatterDuration = 7.0f;  // 7 seconds scatter
        ghost.chaseDuration = 20.0f;   // 20 seconds chase
        ghost.ghostID = i;
        ghosts.push_back(ghost);
    }
    
    return ghosts;
}

// ─── Get corner targets for scatter mode ───────────────────────────────────
sf::Vector2f getScatterTarget(int ghostID) {
    switch(ghostID) {
        case 0: return sf::Vector2f(1 * TILE_SIZE + TILE_SIZE/2, 1 * TILE_SIZE + TILE_SIZE/2);           // top-left (red)
        case 1: return sf::Vector2f(26 * TILE_SIZE + TILE_SIZE/2, 1 * TILE_SIZE + TILE_SIZE/2);          // top-right (pink)
        case 2: return sf::Vector2f(1 * TILE_SIZE + TILE_SIZE/2, 27 * TILE_SIZE + TILE_SIZE/2);          // bottom-left (blue)
        case 3: return sf::Vector2f(26 * TILE_SIZE + TILE_SIZE/2, 27 * TILE_SIZE + TILE_SIZE/2);         // bottom-right (orange)
        default: return sf::Vector2f(14 * TILE_SIZE + TILE_SIZE/2, 14 * TILE_SIZE + TILE_SIZE/2);
    }
}

// ─── AI: Find best direction toward target ──────────────────────────────────
Direction getDirectionTowardTarget(sf::Vector2f currentPos, sf::Vector2f targetPos, float uiOffset) {
    int currX = (int)(currentPos.x / TILE_SIZE);
    int currY = (int)((currentPos.y - uiOffset) / TILE_SIZE);
    
    int targetX = (int)(targetPos.x / TILE_SIZE);
    int targetY = (int)((targetPos.y - uiOffset) / TILE_SIZE);
    
    float dx = targetPos.x - currentPos.x;
    float dy = targetPos.y - currentPos.y;
    
    // Check all 4 directions and pick the one that reduces distance most
    Direction bestDir = Direction::None;
    float bestDist = std::hypot(dx, dy);
    
    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
    sf::Vector2f dirVecs[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    for (int i = 0; i < 4; i++) {
        int nextX = currX + (int)dirVecs[i].x;
        int nextY = currY + (int)dirVecs[i].y;
        
        if (!isWall(nextX, nextY)) {  // Check if next tile is walkable
            sf::Vector2f nextPos(nextX * TILE_SIZE + TILE_SIZE/2, 
                                nextY * TILE_SIZE + TILE_SIZE/2 + uiOffset);
            float newDist = std::hypot(targetPos.x - nextPos.x, targetPos.y - nextPos.y);
            if (newDist < bestDist) {
                bestDist = newDist;
                bestDir = dirs[i];
            }
        }
    }
    
    return bestDir;
}

// ─── Update ghost mode (chase/scatter cycling) ──────────────────────────────
void updateGhostMode(Ghost& ghost) {
    switch(ghost.mode) {
        case GhostMode::CHASE:
            if (ghost.modeTimer.getElapsedTime().asSeconds() > ghost.chaseDuration) {
                ghost.mode = GhostMode::SCATTER;
                ghost.modeTimer.restart();
            }
            break;
        case GhostMode::SCATTER:
            if (ghost.modeTimer.getElapsedTime().asSeconds() > ghost.scatterDuration) {
                ghost.mode = GhostMode::CHASE;
                ghost.modeTimer.restart();
            }
            break;
        case GhostMode::FRIGHTENED:
            // Frightened mode is triggered externally; duration managed separately
            break;
    }
}

// ─── Update all ghosts ──────────────────────────────────────────────────────
void updateGhosts(std::vector<Ghost>& ghosts, const Entity& pacman, float dt, float uiOffset) {
    for (auto& ghost : ghosts) {
        updateGhostMode(ghost);
        
        Direction nextDir = Direction::None;
        
        if (ghost.mode == GhostMode::CHASE) {
            nextDir = getDirectionTowardTarget(ghost.entity.pos, pacman.pos, uiOffset);
        } 
        else if (ghost.mode == GhostMode::SCATTER) {
            sf::Vector2f scatterTarget = getScatterTarget(ghost.ghostID);
            nextDir = getDirectionTowardTarget(ghost.entity.pos, scatterTarget, uiOffset);
        }
        else if (ghost.mode == GhostMode::FRIGHTENED) {
            // Random movement
            Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
            int randIdx = std::rand() % 4;
            nextDir = dirs[randIdx];
            
            int currX = (int)(ghost.entity.pos.x / TILE_SIZE);
            int currY = (int)((ghost.entity.pos.y - uiOffset) / TILE_SIZE);
            sf::Vector2f dirVec = getDirectionVector(nextDir);
            if (isWall(currX + (int)dirVec.x, currY + (int)dirVec.y)) {
                nextDir = Direction::None;
            }
        }
        
        ghost.entity.currentDir = nextDir;
        
        // Move ghost at slightly slower speed than Pacman
        float ghostSpeed = PACMAN_SPEED * 0.85f;
        moveEntity(ghost.entity, ghostSpeed, dt, uiOffset, baseMap[0].size());
    }
}

// ─── Check collision: do any ghosts touch Pacman? ──────────────────────────
bool checkGhostCollision(Entity& pacman, std::vector<Ghost>& ghosts, 
                         bool hasShield, bool hasPower, sf::Sound& sEat, sf::Sound& sDth) {
    for (auto& ghost : ghosts) {
        float dist = calcDist(pacman.pos, ghost.entity.pos);
        
        if (dist < TILE_SIZE * 0.6f) {  // Collision threshold
            if (ghost.mode == GhostMode::FRIGHTENED) {
                // Pacman eats the ghost
                ghost.entity.pos = getScatterTarget(ghost.ghostID) + sf::Vector2f(0, 3 * TILE_SIZE);
                ghost.mode = GhostMode::SCATTER;
                sEat.play();
                return false;  // No life lost
            }
            else if (!hasShield) {
                // Ghost caught Pacman
                sDth.play();
                return true;  // Life lost!
            }
            // If shield is active, nothing happens to Pacman
        }
    }
    return false;  // No collision
}

// ─── Draw ghosts ───────────────────────────────────────────────────────────
void drawGhosts(sf::RenderWindow& window, const std::vector<Ghost>& ghosts, float animTime) {
    for (const auto& ghost : ghosts) {
        float bobOffset = std::sin(animTime * 3.0f) * 1.5f;  // bobbing animation
        
        // Ghost body (rounded rectangle)
        sf::CircleShape body(TILE_SIZE * 0.35f);
        body.setPosition(ghost.entity.pos - sf::Vector2f(TILE_SIZE * 0.35f, TILE_SIZE * 0.35f + bobOffset));
        body.setFillColor(ghost.entity.color);
        
        // Darken if frightened
        if (ghost.mode == GhostMode::FRIGHTENED) {
            body.setFillColor(sf::Color::Blue);
        }
        
        window.draw(body);
        
        // Eyes (two small circles)
        sf::CircleShape eye1(2.f);
        eye1.setPosition(ghost.entity.pos - sf::Vector2f(6.f, 8.f + bobOffset));
        eye1.setFillColor(sf::Color::White);
        
        sf::CircleShape eye2(2.f);
        eye2.setPosition(ghost.entity.pos - sf::Vector2f(-2.f, 8.f + bobOffset));
        eye2.setFillColor(sf::Color::White);
        
        window.draw(eye1);
        window.draw(eye2);
    }
}

// ─── Trigger frightened mode (when Pacman gets power) ─────────────────────
void frightenGhosts(std::vector<Ghost>& ghosts, float duration = 8.0f) {
    for (auto& ghost : ghosts) {
        ghost.mode = GhostMode::FRIGHTENED;
        ghost.modeTimer.restart();
        ghost.scatterDuration = duration;  // Reuse timer for frightened duration
    }
}

// ─── End frightened mode ────────────────────────────────────────────────────
void unfrightenGhosts(std::vector<Ghost>& ghosts) {
    for (auto& ghost : ghosts) {
        if (ghost.mode == GhostMode::FRIGHTENED) {
            ghost.mode = GhostMode::SCATTER;
            ghost.modeTimer.restart();
        }
    }
}