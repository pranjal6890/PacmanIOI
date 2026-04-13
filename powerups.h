#pragma once
#include "globals.h"

inline void drawHealthPowerup(sf::RenderWindow &window, sf::Vector2f pos,
                              float animTime)
{
  float s = 5.0f * (1.0f + 0.18f * std::sin(animTime * 5.0f));
  window.draw(createThickArc(pos, s * 1.6f, 2.5f, 0.0f, 360.0f,
                             sf::Color(255, 80, 80, 120)));
  sf::CircleShape lobe(s * 0.6f);
  lobe.setFillColor(sf::Color(255, 60, 100));
  lobe.setOrigin({s * 0.6f, s * 0.6f});
  lobe.setPosition(pos + sf::Vector2f(-s * 0.32f, -s * 0.3f));
  window.draw(lobe);
  lobe.setPosition(pos + sf::Vector2f(s * 0.32f, -s * 0.3f));
  window.draw(lobe);
  sf::VertexArray tri(sf::PrimitiveType::Triangles, 3);
  sf::Color hc(255, 60, 100);
  tri[0] = sf::Vertex{pos + sf::Vector2f(-s * 0.9f, -s * 0.1f), hc};
  tri[1] = sf::Vertex{pos + sf::Vector2f(s * 0.9f, -s * 0.1f), hc};
  tri[2] = sf::Vertex{pos + sf::Vector2f(0.0f, s * 0.9f), hc};
  window.draw(tri);
}

inline void drawPowerPowerup(sf::RenderWindow &window, sf::Vector2f pos,
                             float animTime)
{
  float s = 5.5f * (1.0f + 0.15f * std::sin(animTime * 6.0f));
  window.draw(createThickArc(pos, s * 1.6f, 2.5f, 0.0f, 360.0f,
                             sf::Color(180, 80, 255, 130)));
  for (int i = 0; i < 4; ++i)
  {
    float angle = animTime * 3.0f + i * (2 * 3.14159f / 4.0f);
    sf::Vector2f sp = pos + sf::Vector2f(std::cos(angle) * s * 0.9f,
                                         std::sin(angle) * s * 0.9f);
    sf::VertexArray star(sf::PrimitiveType::Triangles, 24);
    sf::Color sc(255, 220, 0);
    float sr = s * 0.38f;
    for (int j = 0; j < 8; ++j)
    {
      float a1 = j * 2 * 3.14159f / 8.0f,
            a2 = (j + 1) * 2 * 3.14159f / 8.0f;
      float r1 = (j % 2 == 0) ? sr : sr * 0.45f,
            r2 = ((j + 1) % 2 == 0) ? sr : sr * 0.45f;
      star[j * 3 + 0] = sf::Vertex{sp, sc};
      star[j * 3 + 1] = sf::Vertex{
          sp + sf::Vector2f(std::cos(a1) * r1, std::sin(a1) * r1), sc};
      star[j * 3 + 2] = sf::Vertex{
          sp + sf::Vector2f(std::cos(a2) * r2, std::sin(a2) * r2), sc};
    }
    window.draw(star);
  }
  sf::CircleShape dot(s * 0.25f);
  dot.setOrigin({s * 0.25f, s * 0.25f});
  dot.setPosition(pos);
  dot.setFillColor(sf::Color(255, 220, 0, 200));
  window.draw(dot);
}

inline void drawShieldPowerup(sf::RenderWindow &window, sf::Vector2f pos,
                              float animTime)
{
  float s = 6.0f * (1.0f + 0.15f * std::sin(animTime * 4.5f));
  window.draw(createThickArc(pos, s * 1.6f, 2.5f, 0.0f, 360.0f,
                             sf::Color(60, 255, 120, 130)));
  sf::ConvexShape shield(6);
  shield.setPoint(0, {0, -s});
  shield.setPoint(1, {s * 0.87f, -s * 0.5f});
  shield.setPoint(2, {s * 0.87f, s * 0.5f});
  shield.setPoint(3, {0, s});
  shield.setPoint(4, {-s * 0.87f, s * 0.5f});
  shield.setPoint(5, {-s * 0.87f, -s * 0.5f});
  shield.setFillColor(sf::Color(40, 200, 80, 200));
  shield.setOutlineColor(sf::Color(120, 255, 160));
  shield.setOutlineThickness(1.5f);
  shield.setPosition(pos);
  window.draw(shield);
  sf::RectangleShape bar({s * 0.9f, s * 0.28f});
  bar.setOrigin({s * 0.45f, s * 0.14f});
  bar.setPosition(pos);
  bar.setFillColor(sf::Color(200, 255, 220));
  window.draw(bar);
  bar.setSize({s * 0.28f, s * 0.9f});
  bar.setOrigin({s * 0.14f, s * 0.45f});
  window.draw(bar);
}

inline void drawShieldAura(sf::RenderWindow &window, sf::Vector2f pos,
                           float animTime)
{
  float r = 11.0f * (1.0f + 0.12f * std::sin(animTime * 8.0f));
  window.draw(createThickArc(pos, r + 3.0f, 3.0f, 0.0f, 360.0f,
                             sf::Color(60, 255, 120, 70)));
  window.draw(createThickArc(pos, r, 2.5f, 0.0f, 360.0f,
                             sf::Color(80, 255, 140, 220)));
  for (int i = 0; i < 6; ++i)
  {
    float angle = animTime * 1.5f + i * (2 * 3.14159f / 6.0f);
    sf::CircleShape node(2.0f);
    node.setOrigin({2, 2});
    node.setPosition(pos + sf::Vector2f(std::cos(angle) * r,
                                        std::sin(angle) * r));
    node.setFillColor(sf::Color(180, 255, 200));
    window.draw(node);
  }
}
