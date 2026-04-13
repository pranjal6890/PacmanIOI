#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum class Direction
{
  None,
  Up,
  Down,
  Left,
  Right
};

enum class GameState
{
  Playing,
  GameOver,
  GameWon,
  Paused
};

enum class EndChoice
{
  PlayAgain,
  Quit
};

enum class PowerupType
{
  Health,
  Power,
  Shield
};

const float TILE_SIZE = 20.0f;
const float PACMAN_SPEED = 120.0f;

struct Powerup
{
  sf::Vector2f pos;
  PowerupType type;
  bool active = true;
};

struct Entity
{
  sf::Vector2f pos;
  Direction currentDir = Direction::None;
  Direction queuedDir = Direction::None;
  sf::Color color;
};

inline std::vector<std::string> baseMap = {
    "############################", "#            ##            #",
    "# #### ##### ## ##### #### #", "# #### ##### ## ##### #### #",
    "# #### ##### ## ##### #### #", "#                          #",
    "# #### ## ######## ## #### #", "# #### ## ######## ## #### #",
    "#      ##    ##    ##      #", "###### ##### ## ##### ######",
    "xxxxx# ##### ## ##### #xxxxx", "xxxxx# ##          ## #xxxxx",
    "xxxxx# ## ###--### ## #xxxxx", "###### ## #xxxxxx# ## ######",
    "          #xxxxxx#          ", "###### ## #xxxxxx# ## ######",
    "xxxxx# ## ######## ## #xxxxx", "xxxxx# ##          ## #xxxxx",
    "xxxxx# ## ######## ## #xxxxx", "###### ## ######## ## ######",
    "#            ##            #", "# #### ##### ## ##### #### #",
    "# #### ##### ## ##### #### #", "#   ##                ##   #",
    "### ## ## ######## ## ## ###", "### ## ## ######## ## ## ###",
    "#      ##    ##    ##      #", "# ########## ## ########## #",
    "# ########## ## ########## #", "#                          #",
    "############################"};

inline bool isWall(int x, int y)
{
  int mapW = (int)baseMap[0].size();

  while (x < 0)
    x += mapW;
  while (x >= mapW)
    x -= mapW;

  if (y < 0 || y >= (int)baseMap.size())
    return true;

  return baseMap[y][x] == '#' || baseMap[y][x] == '-';
}

inline sf::Vector2f getDirectionVector(Direction dir)
{
  switch (dir)
  {
  case Direction::Up:
    return {0, -1};
  case Direction::Down:
    return {0, 1};
  case Direction::Left:
    return {-1, 0};
  case Direction::Right:
    return {1, 0};
  default:
    return {0, 0};
  }
}

inline float calcDist(sf::Vector2f a, sf::Vector2f b)
{
  return std::hypot(a.x - b.x, a.y - b.y);
}

inline sf::VertexArray createThickArc(sf::Vector2f center, float radius,
                                      float thickness, float startAngle,
                                      float endAngle, sf::Color color)
{
  const int points = 15;
  sf::VertexArray arc(sf::PrimitiveType::Triangles, (points - 1) * 6);
  for (int i = 0; i < points - 1; ++i)
  {
    float a1 = startAngle + (endAngle - startAngle) * (i / (float)(points - 1));
    float a2 = startAngle + (endAngle - startAngle) * ((i + 1) / (float)(points - 1));
    float r1 = a1 * 3.14159f / 180.0f, r2 = a2 * 3.14159f / 180.0f;
    sf::Vector2f d1(std::cos(r1), std::sin(r1)), d2(std::cos(r2), std::sin(r2));
    sf::Vector2f p1o = center + d1 * radius,
                 p1i = center + d1 * (radius - thickness);
    sf::Vector2f p2o = center + d2 * radius,
                 p2i = center + d2 * (radius - thickness);
    int idx = i * 6;
    arc[idx + 0] = sf::Vertex{p1i, color};
    arc[idx + 1] = sf::Vertex{p1o, color};
    arc[idx + 2] = sf::Vertex{p2o, color};
    arc[idx + 3] = sf::Vertex{p1i, color};
    arc[idx + 4] = sf::Vertex{p2o, color};
    arc[idx + 5] = sf::Vertex{p2i, color};
  }
  return arc;
}