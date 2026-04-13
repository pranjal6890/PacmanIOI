#pragma once

#include "globals.h"

inline void handlePacmanInput(Entity &pacman)
{
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    pacman.queuedDir = Direction::Up;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    pacman.queuedDir = Direction::Down;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    pacman.queuedDir = Direction::Left;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    pacman.queuedDir = Direction::Right;
}

inline void moveEntity(Entity &e, float spd, float dt,
                       float uiOffset, unsigned mapW)
{
  int gx = (int)(e.pos.x / TILE_SIZE);
  int gy = (int)((e.pos.y - uiOffset) / TILE_SIZE);

  sf::Vector2f tc(gx * TILE_SIZE + TILE_SIZE / 2,
                  gy * TILE_SIZE + TILE_SIZE / 2 + uiOffset);

  sf::Vector2f toC = tc - e.pos;
  sf::Vector2f cDir = getDirectionVector(e.currentDir);
  bool towards = (toC.x * cDir.x + toC.y * cDir.y) >
                 0;

  if (calcDist(e.pos, tc) < 0.1f ||
      (towards && calcDist(e.pos, tc) <= spd * dt))
  {
    e.pos = tc;
    auto qv = getDirectionVector(e.queuedDir);
    if (!isWall(gx + (int)qv.x, gy + (int)qv.y))
      e.currentDir = e.queuedDir;
    else if (isWall(gx + (int)getDirectionVector(e.currentDir).x,
                    gy + (int)getDirectionVector(e.currentDir).y))
      e.currentDir = Direction::None;
    e.pos += getDirectionVector(e.currentDir) * spd * dt;
  }
  else
  {
    e.pos += cDir * spd * dt;
  }

  if (e.pos.x < 0)
    e.pos.x += mapW * TILE_SIZE;
  if (e.pos.x >= (float)mapW * TILE_SIZE)
    e.pos.x -= mapW * TILE_SIZE;
}
