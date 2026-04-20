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

#include "ghost_direction.h"

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

inline bool isWall(int x, int y, bool isGhost = false)
{
  int map_width_temp = (int)baseMap[0].size();

  while (x < 0) {
    x = x + map_width_temp;
  }
  while (x >= map_width_temp) {
    x = x - map_width_temp;
  }

  if (y < 0 || y >= (int)baseMap.size()) {
    return true;
  }


  if (isGhost == true) {
    if (baseMap[y][x] == '-') {
      return false;
    }
    if (y >= 12 && y <= 15 && x >= 10 && x <= 17) {
      if (baseMap[12][13] == '#') {
        return true;
      }
    }
  }

  if (baseMap[y][x] == '#') {
    return true;
  }
  if (baseMap[y][x] == '-') {
    return true;
  }

  return false;
}



inline sf::VertexArray createThickArc(sf::Vector2f center, float radius,
                                      float thickness, float startAngle,
                                      float endAngle, sf::Color color)
{
  const int number_of_points = 15;

  sf::VertexArray arc_shape(sf::PrimitiveType::Triangles, (number_of_points - 1) * 6);


  int i = 0;
  while (i < number_of_points - 1)
  {
    float angle_1_temp = startAngle + (endAngle - startAngle) * (i / (float)(number_of_points - 1));
    float angle_2_temp = startAngle + (endAngle - startAngle) * ((i + 1) / (float)(number_of_points - 1));

    float radians_1 = angle_1_temp * 3.14159f / 180.0f;
    float radians_2 = angle_2_temp * 3.14159f / 180.0f;

    sf::Vector2f direction_1;
    direction_1.x = std::cos(radians_1);
    direction_1.y = std::sin(radians_1);

    sf::Vector2f direction_2;
    direction_2.x = std::cos(radians_2);
    direction_2.y = std::sin(radians_2);

    sf::Vector2f outer_point_1;
    outer_point_1.x = center.x + direction_1.x * radius;
    outer_point_1.y = center.y + direction_1.y * radius;

    sf::Vector2f inner_point_1;
    inner_point_1.x = center.x + direction_1.x * (radius - thickness);
    inner_point_1.y = center.y + direction_1.y * (radius - thickness);

    sf::Vector2f outer_point_2;
    outer_point_2.x = center.x + direction_2.x * radius;
    outer_point_2.y = center.y + direction_2.y * radius;

    sf::Vector2f inner_point_2;
    inner_point_2.x = center.x + direction_2.x * (radius - thickness);
    inner_point_2.y = center.y + direction_2.y * (radius - thickness);

    int idx = i * 6;
    arc_shape[idx + 0] = sf::Vertex{inner_point_1, color};
    arc_shape[idx + 1] = sf::Vertex{outer_point_1, color};
    arc_shape[idx + 2] = sf::Vertex{outer_point_2, color};
    arc_shape[idx + 3] = sf::Vertex{inner_point_1, color};
    arc_shape[idx + 4] = sf::Vertex{outer_point_2, color};
    arc_shape[idx + 5] = sf::Vertex{inner_point_2, color};

    i = i + 1;
  }

  return arc_shape;
}
