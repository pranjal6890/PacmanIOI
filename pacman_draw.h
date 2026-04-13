#pragma once

#include "globals.h"

inline void drawPacman(sf::RenderWindow &window, sf::Vector2f pos,
                       sf::Color color, Direction dir, float animTime) {
  float radius = TILE_SIZE / 2.0f * 0.8f;
  float mouthAngle = std::abs(std::sin(animTime * 15.0f)) * 70.0f;
  float startAngle = 0;
  if (dir == Direction::Down)
    startAngle = 90;
  else if (dir == Direction::Left)
    startAngle = 180;
  else if (dir == Direction::Up)
    startAngle = 270;
  float half = mouthAngle / 2.0f;
  const int pts = 30;
  sf::VertexArray shape(sf::PrimitiveType::Triangles, pts * 3);
  for (int i = 0; i < pts; ++i) {
    float a1 = (startAngle + half + (360.0f - mouthAngle) * (i / (float)pts)) *
               3.14159f / 180.f;
    float a2 =
        (startAngle + half + (360.0f - mouthAngle) * ((i + 1) / (float)pts)) *
        3.14159f / 180.f;
    shape[i * 3 + 0] = sf::Vertex{pos, color};
    shape[i * 3 + 1] = sf::Vertex{
        pos + sf::Vector2f(std::cos(a1) * radius, std::sin(a1) * radius),
        color};
    shape[i * 3 + 2] = sf::Vertex{
        pos + sf::Vector2f(std::cos(a2) * radius, std::sin(a2) * radius),
        color};
  }
  window.draw(shape);
}

