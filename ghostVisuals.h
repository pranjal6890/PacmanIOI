#pragma once

#include "ghost_direction_and_visual_p1.h"
#include "pacman_movement.h"
#include <vector>
#include <cmath>
#include <queue>
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

inline void drawGhost(sf::RenderWindow &window, sf::Vector2f pos,
                      sf::Color color, Direction dir,
                      int mood, float animTime, float tileSize)
{
    float radius = tileSize / 2.0f * 0.8f;

    sf::CircleShape dome(radius);
    dome.setOrigin({radius, radius});
    dome.setPosition(pos);
    dome.setFillColor(color);
    window.draw(dome);

    sf::RectangleShape body({radius * 2, radius});
    body.setOrigin({radius, 0});
    body.setPosition(pos);
    body.setFillColor(color);
    window.draw(body);

    float offset = std::sin(animTime * 15.0f) * 1.5f;
    sf::VertexArray tri(sf::PrimitiveType::Triangles, 9);
    float w = (radius * 2) / 3.0f;
    for (int i = 0; i < 3; ++i) {
        float sx = -radius + i * w;
        tri[i*3+0] = sf::Vertex{pos + sf::Vector2f(sx,           radius),                                    color};
        tri[i*3+1] = sf::Vertex{pos + sf::Vector2f(sx + w/2,     radius + 3 + ((i%2==0) ? offset : -offset)), color};
        tri[i*3+2] = sf::Vertex{pos + sf::Vector2f(sx + w,       radius),                                    color};
    }
    window.draw(tri);

    sf::Vector2f lk(0, 0);
    if (dir == Direction::Left)  lk.x = -1.5f;
    if (dir == Direction::Right) lk.x =  1.5f;
    if (dir == Direction::Up)    lk.y = -1.5f;
    if (dir == Direction::Down)  lk.y =  1.5f;

    sf::CircleShape eye(3.0f);
    eye.setFillColor(sf::Color::White);
    eye.setOrigin({1.5f, 1.5f});

    sf::CircleShape pupil(1.5f);
    pupil.setFillColor((mood == 1) ? sf::Color::Red : sf::Color::Blue);
    pupil.setOrigin({0.75f, 0.75f});

    for (int side = -1; side <= 1; side += 2) {
        eye.setPosition(pos + sf::Vector2f(side * 3.5f, -2.0f) + lk);
        window.draw(eye);
        pupil.setPosition(eye.getPosition() + lk);
        window.draw(pupil);
    }

    if (mood == 1) {
        sf::RectangleShape brow({5.0f, 1.5f});
        brow.setFillColor(sf::Color::Black);
        brow.setOrigin({2.5f, 0.75f});
        brow.setPosition(pos + sf::Vector2f(-3.5f, -5.5f));
        brow.setRotation(sf::degrees(25));
        window.draw(brow);
        brow.setPosition(pos + sf::Vector2f(3.5f, -5.5f));
        brow.setRotation(sf::degrees(-25));
        window.draw(brow);
    }
    else if (mood == 2) {
        sf::ConvexShape horn(3);
        horn.setPoint(0, {0, 0});
        horn.setPoint(1, {2, 0});
        horn.setPoint(2, {1, -4});
        horn.setFillColor(sf::Color::Red);
        horn.setPosition(pos + sf::Vector2f(-5, -8));
        window.draw(horn);
        horn.setPosition(pos + sf::Vector2f(3, -8));
        window.draw(horn);
    }
}

inline void drawConfusedStars(sf::RenderWindow &window,
                               sf::Vector2f ghostPos, float animTime)
{
    sf::Vector2f center = ghostPos + sf::Vector2f(0, -15.0f);
    float orbitR = 8.0f, sr = 2.8f;

    for (int i = 0; i < 3; ++i) {
        float angle = animTime * 5.0f + i * (2 * 3.14159f / 3.0f);
        sf::Vector2f sp = center + sf::Vector2f(
            std::cos(angle) * orbitR,
            std::sin(angle) * orbitR * 0.45f
        );

        sf::VertexArray star(sf::PrimitiveType::Triangles, 24);
        sf::Color sc(255, 230, 0);
        for (int j = 0; j < 8; ++j) {
            float a1 = j     * 2 * 3.14159f / 8.0f;
            float a2 = (j+1) * 2 * 3.14159f / 8.0f;
            float r1 = (j   % 2 == 0) ? sr : sr * 0.4f;
            float r2 = ((j+1)%2 == 0) ? sr : sr * 0.4f;
            star[j*3+0] = sf::Vertex{sp, sc};
            star[j*3+1] = sf::Vertex{sp + sf::Vector2f(std::cos(a1)*r1, std::sin(a1)*r1), sc};
            star[j*3+2] = sf::Vertex{sp + sf::Vector2f(std::cos(a2)*r2, std::sin(a2)*r2), sc};
        }
        window.draw(star);
    }
}

// ─── Ghost Initialization ───────────────────────────────────────────────────
inline std::vector<Ghost> initializeGhosts(float uiOffset) {
    std::vector<Ghost> ghosts;
    
    sf::Vector2f spawnPoints[] = {
        sf::Vector2f(12 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset),
        sf::Vector2f(13 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset),
        sf::Vector2f(14 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset),
        sf::Vector2f(15 * TILE_SIZE + TILE_SIZE/2, 13 * TILE_SIZE + TILE_SIZE/2 + uiOffset)
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
        ghost.scatterDuration = 7.0f;
        ghost.chaseDuration = 20.0f;
        ghost.ghostID = i;
        ghosts.push_back(ghost);
    }
    
    return ghosts;
}

// ─── Get corner targets for scatter mode ───────────────────────────────────
inline sf::Vector2f getScatterTarget(int ghostID) {
    switch(ghostID) {
        case 0: return sf::Vector2f(1 * TILE_SIZE + TILE_SIZE/2, 1 * TILE_SIZE + TILE_SIZE/2);
        case 1: return sf::Vector2f(26 * TILE_SIZE + TILE_SIZE/2, 1 * TILE_SIZE + TILE_SIZE/2);
        case 2: return sf::Vector2f(1 * TILE_SIZE + TILE_SIZE/2, 27 * TILE_SIZE + TILE_SIZE/2);
        case 3: return sf::Vector2f(26 * TILE_SIZE + TILE_SIZE/2, 27 * TILE_SIZE + TILE_SIZE/2);
        default: return sf::Vector2f(14 * TILE_SIZE + TILE_SIZE/2, 14 * TILE_SIZE + TILE_SIZE/2);
    }
}

// ─── AI: Find best direction toward target ──────────────────────────────────
inline Direction getDirectionTowardTarget(sf::Vector2f currentPos, sf::Vector2f targetPos, float uiOffset) {
    int currX = (int)(currentPos.x / TILE_SIZE);
    int currY = (int)((currentPos.y - uiOffset) / TILE_SIZE);
    
    float dx = targetPos.x - currentPos.x;
    float dy = targetPos.y - currentPos.y;
    
    Direction bestDir = Direction::None;
    float bestDist = std::hypot(dx, dy);
    
    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
    sf::Vector2f dirVecs[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    for (int i = 0; i < 4; i++) {
        int nextX = currX + (int)dirVecs[i].x;
        int nextY = currY + (int)dirVecs[i].y;
        
        if (!isWall(nextX, nextY)) {
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
inline void updateGhostMode(Ghost& ghost) {
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
            break;
    }
}

// ─── Update all ghosts ──────────────────────────────────────────────────────
inline void updateGhosts(std::vector<Ghost>& ghosts, const Entity& pacman, float dt, float uiOffset) {
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
        
        float ghostSpeed = PACMAN_SPEED * 0.85f;
        moveEntity(ghost.entity, ghostSpeed, dt, uiOffset, baseMap[0].size());
    }
}

// ─── Check collision: do any ghosts touch Pacman? ──────────────────────────
inline bool checkGhostCollision(Entity& pacman, std::vector<Ghost>& ghosts, 
                         bool hasShield, bool hasPower, sf::Sound& sEat, sf::Sound& sDth) {
    for (auto& ghost : ghosts) {
        float dist = calcDist(pacman.pos, ghost.entity.pos);
        
        if (dist < TILE_SIZE * 0.6f) {
            sDth.play();
            return true;
        }
    }
    return false;
}

// ─── Draw ghosts ───────────────────────────────────────────────────────────
inline void drawGhosts(sf::RenderWindow& window, const std::vector<Ghost>& ghosts, float animTime) {
    for (const auto& ghost : ghosts) {
        int mood = 0; // 0 = normal, 1 = angry, 2 = frightened
        sf::Color drawCol = ghost.entity.color;
        
        if (ghost.mode == GhostMode::FRIGHTENED) {
            mood = 2; // Frigtened (horns)
            drawCol = sf::Color::Blue;
        } else if (ghost.mode == GhostMode::CHASE) {
            mood = 1; // Angry (brows)
        }
        
        drawGhost(window, ghost.entity.pos, drawCol, ghost.entity.currentDir, mood, animTime, TILE_SIZE);
        
        if (ghost.mode == GhostMode::FRIGHTENED) {
            drawConfusedStars(window, ghost.entity.pos, animTime);
        }
    }
}

// ─── Trigger frightened mode (when Pacman gets power) ─────────────────────
inline void frightenGhosts(std::vector<Ghost>& ghosts, float duration = 8.0f) {
    for (auto& ghost : ghosts) {
        ghost.mode = GhostMode::FRIGHTENED;
        ghost.modeTimer.restart();
        ghost.scatterDuration = duration;
    }
}

// ─── End frightened mode ────────────────────────────────────────────────────
inline void unfrightenGhosts(std::vector<Ghost>& ghosts) {
    for (auto& ghost : ghosts) {
        if (ghost.mode == GhostMode::FRIGHTENED) {
            ghost.mode = GhostMode::SCATTER;
            ghost.modeTimer.restart();
        }
    }
}