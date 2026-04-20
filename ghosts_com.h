
#pragma once

#include "ghost_direction.h"
#include "ghost_visuals.h"
#include "pacman_movement.h"
#include <iostream>
#include <memory>


inline void drawConfusedStars(sf::RenderWindow &window, sf::Vector2f ghostPos,
                              float animTime) {
  sf::Vector2f center_point;
  center_point.x = ghostPos.x + 0;
  center_point.y = ghostPos.y + -15.0f;

  float orbit_radius_temp = 8.0f;
  float star_radius_temp = 2.8f;

  int star_number = 0;
  while (star_number < 3) {
    float angle_temp = animTime * 5.0f + star_number * (2 * 3.14159f / 3.0f);

    sf::Vector2f star_position;
    star_position.x = center_point.x + std::cos(angle_temp) * orbit_radius_temp;
    star_position.y = center_point.y + std::sin(angle_temp) * orbit_radius_temp * 0.45f;

    sf::VertexArray star_triangles(sf::PrimitiveType::Triangles, 24);
    sf::Color star_color(255, 230, 0);

    int j = 0;
    while (j < 8) {
      float a1 = j * 2 * 3.14159f / 8.0f;
      float a2 = (j + 1) * 2 * 3.14159f / 8.0f;

      float r1 = star_radius_temp;
      if (j % 2 != 0) {
        r1 = star_radius_temp * 0.4f;
      }
      float r2 = star_radius_temp;
      if ((j + 1) % 2 != 0) {
        r2 = star_radius_temp * 0.4f;
      }

      star_triangles[j * 3 + 0] = sf::Vertex{star_position, star_color};

      sf::Vector2f p1;
      p1.x = star_position.x + std::cos(a1) * r1;
      p1.y = star_position.y + std::sin(a1) * r1;
      star_triangles[j * 3 + 1] = sf::Vertex{p1, star_color};

      sf::Vector2f p2;
      p2.x = star_position.x + std::cos(a2) * r2;
      p2.y = star_position.y + std::sin(a2) * r2;
      star_triangles[j * 3 + 2] = sf::Vertex{p2, star_color};

      j = j + 1;
    }
    window.draw(star_triangles);

    star_number = star_number + 1;
  }
}


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
    if (respawning == true) {
      sf::Vector2f respawn_target;
      respawn_target.x = 14.f;
      respawn_target.y = 11.f;
      return respawn_target;
    }
    if (gy >= 11 && gy <= 15 && gx >= 10 && gx <= 17) {
      sf::Vector2f exit_target;
      exit_target.x = 14.f;
      exit_target.y = 9.f;
      return exit_target;
    }
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


inline sf::Vector2f getScatterTarget(int ghostID) {
  if (ghostID == 0) {
    return sf::Vector2f(26.f, 1.f);
  }
  else if (ghostID == 1) {
    return sf::Vector2f(1.f, 1.f);
  }
  else if (ghostID == 2) {
    return sf::Vector2f(26.f, 27.f);
  }
  else if (ghostID == 3) {
    return sf::Vector2f(1.f, 27.f);
  }
  else {
    return sf::Vector2f(1.f, 1.f);
  }
}

inline void moveGhost(std::shared_ptr<Ghost> &ghost, float spd, float dt,
                      float uiOffset, unsigned mapW, sf::Vector2f targetTile) {
  int gx = (int)(ghost->pos.x / TILE_SIZE);
  int gy = (int)((ghost->pos.y - uiOffset) / TILE_SIZE);

  sf::Vector2f tile_center;
  tile_center.x = gx * TILE_SIZE + TILE_SIZE / 2;
  tile_center.y = gy * TILE_SIZE + TILE_SIZE / 2 + uiOffset;

  sf::Vector2f to_center;
  to_center.x = tile_center.x - ghost->pos.x;
  to_center.y = tile_center.y - ghost->pos.y;

  sf::Vector2f current_dir_vec = getDirectionVector(ghost->currentDir);

  float dot_product = to_center.x * current_dir_vec.x + to_center.y * current_dir_vec.y;
  bool moving_towards = false;
  if (dot_product > 0) {
    moving_towards = true;
  }


  float dist = calcDist(ghost->pos, tile_center);

  bool should_snap = false;
  if (dist < 0.1f) {
    should_snap = true;
  }
  if (moving_towards == true && dist <= spd * dt) {
    should_snap = true;
  }

  if (should_snap == true) {
    ghost->pos = tile_center;

    Direction best_direction = Direction::None;
    float best_distance = 999999.0f;

    Direction all_directions[4];
    all_directions[0] = Direction::Up;
    all_directions[1] = Direction::Down;
    all_directions[2] = Direction::Left;
    all_directions[3] = Direction::Right;

    sf::Vector2f direction_vectors[4];
    direction_vectors[0] = sf::Vector2f(0, -1);
    direction_vectors[1] = sf::Vector2f(0, 1);
    direction_vectors[2] = sf::Vector2f(-1, 0);
    direction_vectors[3] = sf::Vector2f(1, 0);

    Direction reverse_direction = Direction::None;
    if (ghost->currentDir == Direction::Up) {
      reverse_direction = Direction::Down;
    }
    if (ghost->currentDir == Direction::Down) {
      reverse_direction = Direction::Up;
    }
    if (ghost->currentDir == Direction::Left) {
      reverse_direction = Direction::Right;
    }
    if (ghost->currentDir == Direction::Right) {
      reverse_direction = Direction::Left;
    }

    bool already_chosen = false;
    if (ghost->confused == true && ghost->respawning == false) {
      std::vector<Direction> valid_directions_list;
      int i = 0;
      while (i < 4) {
        if (all_directions[i] == reverse_direction && ghost->currentDir != Direction::None) {
          i = i + 1;
          continue;
        }
        int next_x = gx + (int)direction_vectors[i].x;
        int next_y = gy + (int)direction_vectors[i].y;
        if (isWall(next_x, next_y, true) == false) {
          valid_directions_list.push_back(all_directions[i]);
        }
        i = i + 1;
      }
      if (valid_directions_list.size() > 0) {
        int random_index = std::rand() % (int)valid_directions_list.size();
        best_direction = valid_directions_list[random_index];
        already_chosen = true;
      }
    }

    if (already_chosen == false) {
      int i = 0;
      while (i < 4) {
        if (all_directions[i] == reverse_direction && ghost->currentDir != Direction::None) {
          i = i + 1;
          continue;
        }
        int next_x = gx + (int)direction_vectors[i].x;
        int next_y = gy + (int)direction_vectors[i].y;
        if (isWall(next_x, next_y, true) == false) {
          float diff_x = targetTile.x - next_x;
          float diff_y = targetTile.y - next_y;
          float new_distance = std::sqrt(diff_x * diff_x + diff_y * diff_y);
          if (new_distance < best_distance) {
            best_distance = new_distance;
            best_direction = all_directions[i];
          }
        }
        i = i + 1;
      }
    }

    if (best_direction == Direction::None) {
      best_direction = reverse_direction;
    }

    ghost->currentDir = best_direction;

    sf::Vector2f move_vec = getDirectionVector(ghost->currentDir);
    ghost->pos.x = ghost->pos.x + move_vec.x * spd * dt;
    ghost->pos.y = ghost->pos.y + move_vec.y * spd * dt;
  } else {
    ghost->pos.x = ghost->pos.x + current_dir_vec.x * spd * dt;
    ghost->pos.y = ghost->pos.y + current_dir_vec.y * spd * dt;
  }

  if (ghost->pos.x < 0) {
    ghost->pos.x = ghost->pos.x + mapW * TILE_SIZE;
  }
  if (ghost->pos.x >= (float)mapW * TILE_SIZE) {
    ghost->pos.x = ghost->pos.x - mapW * TILE_SIZE;
  }
}

inline void updateGhosts(std::vector<std::shared_ptr<Ghost>> &ghosts,
                         const Entity &pacman, float dt, float uiOffset,
                         float globalTime) {
  float time_in_cycle = std::fmod(globalTime, 27.0f);
  bool scatter_mode_flag = false;
  if (time_in_cycle < 7.0f) {
    scatter_mode_flag = true;
  }

  sf::Vector2f pac_tile_pos;
  pac_tile_pos.x = pacman.pos.x / TILE_SIZE;
  pac_tile_pos.y = (pacman.pos.y - uiOffset) / TILE_SIZE;

  int map_width_int = (int)baseMap[0].size();

  int ghost_index = 0;
  while (ghost_index < (int)ghosts.size()) {
    if (ghosts[ghost_index]->respawning == true) {
      ghosts[ghost_index]->respawnTimer = ghosts[ghost_index]->respawnTimer - dt;
      if (ghosts[ghost_index]->respawnTimer <= 0.f) {
        ghosts[ghost_index]->respawning = false;
        ghosts[ghost_index]->confused = false;
      }
    } else if (ghosts[ghost_index]->confused == true) {
      ghosts[ghost_index]->confuseTimer = ghosts[ghost_index]->confuseTimer - dt;
      if (ghosts[ghost_index]->confuseTimer <= 0.f) {
        ghosts[ghost_index]->confused = false;
      }
    }

    int gx = (int)(ghosts[ghost_index]->pos.x / TILE_SIZE);
    int gy = (int)((ghosts[ghost_index]->pos.y - uiOffset) / TILE_SIZE);

    sf::Vector2f target_tile_temp;
    if (scatter_mode_flag == true && ghosts[ghost_index]->respawning == false) {
      target_tile_temp = getScatterTarget(ghosts[ghost_index]->ghostID);
    } else {
      target_tile_temp = ghosts[ghost_index]->getFinalTargetTile(
          gx, gy, pacman.pos, pac_tile_pos, pacman.currentDir,
          getScatterTarget(ghosts[ghost_index]->ghostID), TILE_SIZE);
    }

    float ghost_speed_temp = PACMAN_SPEED * 0.85f * ghosts[ghost_index]->getSpeedMultiplier();
    if (ghosts[ghost_index]->confused == true) {
      ghost_speed_temp = ghost_speed_temp * 0.5f;
    }
    if (ghosts[ghost_index]->respawning == true) {
      ghost_speed_temp = ghost_speed_temp * 2.0f;
    }

    moveGhost(ghosts[ghost_index], ghost_speed_temp, dt, uiOffset, map_width_int, target_tile_temp);

    ghost_index = ghost_index + 1;
  }

  if (baseMap[12][13] == '-') {
    bool all_out = true;
    int check_idx = 0;
    while (check_idx < (int)ghosts.size()) {
      int check_gx = (int)(ghosts[check_idx]->pos.x / TILE_SIZE);
      int check_gy = (int)((ghosts[check_idx]->pos.y - uiOffset) / TILE_SIZE);
      if (check_gy >= 12 && check_gy <= 15 && check_gx >= 10 && check_gx <= 17) {
        all_out = false;
      }
      check_idx = check_idx + 1;
    }

    if (all_out == true) {
      baseMap[12][13] = '#';
      baseMap[12][14] = '#';
    }
  }
}

inline bool checkGhostCollision(Entity &pacman,
                                std::vector<std::shared_ptr<Ghost>> &ghosts,
                                bool hasShield, bool hasPower, sf::Sound &sEat,
                                sf::Sound &sDth, int &score) {
  int i = 0;
  while (i < (int)ghosts.size()) {
    float distance_temp = calcDist(pacman.pos, ghosts[i]->pos);

    if (distance_temp < TILE_SIZE * 0.6f) {
      if (ghosts[i]->respawning == true) {
        i = i + 1;
        continue;
      }
      if (ghosts[i]->confused == true) {
        sEat.play();
        score = score + 200;
        ghosts[i]->respawning = true;
        ghosts[i]->respawnTimer = 5.0f;
        ghosts[i]->confused = false;
      } else if (hasShield == false) {
        sDth.play();
        return true;
      }
    }

    i = i + 1;
  }
  return false;
}

inline void drawGhostsWrapper(sf::RenderWindow &window,
                              const std::vector<std::shared_ptr<Ghost>> &ghosts,
                              float animTime) {
  int i = 0;
  while (i < (int)ghosts.size()) {
    if (ghosts[i]->respawning == true) {
      drawGhostFace(window, ghosts[i]->pos, ghosts[i]->currentDir, 0);
      i = i + 1;
      continue;
    }

    int mood_temp = 0;
    sf::Color draw_color_temp = ghosts[i]->color;

    if (ghosts[i]->confused == true) {
      mood_temp = 2;
      draw_color_temp = sf::Color::Blue;
    } else {
      mood_temp = 1;
    }

    drawGhost(window, ghosts[i]->pos, draw_color_temp, ghosts[i]->currentDir, mood_temp, animTime,
              TILE_SIZE);

    if (ghosts[i]->confused == true) {
      drawConfusedStars(window, ghosts[i]->pos, animTime);
    }

    i = i + 1;
  }
}

inline void frightenGhosts(std::vector<std::shared_ptr<Ghost>> &ghosts,
                           float duration = 8.0f) {
  int i = 0;
  while (i < (int)ghosts.size()) {
    if (ghosts[i]->respawning == false) {
      ghosts[i]->confused = true;
      ghosts[i]->confuseTimer = duration;
    }
    i = i + 1;
  }
}
