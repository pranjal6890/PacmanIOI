#pragma once


#include "globals.h"


inline std::vector<sf::Vector2f> spawnOrbs(float uiOffset) {
  std::vector<sf::Vector2f> orbs;
  for (unsigned r = 0; r < baseMap.size(); ++r) {
    for (unsigned c = 0; c < baseMap[r].size(); ++c) {
      char tile = baseMap[r][c];
      if (tile != '#' && tile != 'x' && tile != '-') {
        orbs.push_back(sf::Vector2f(c * TILE_SIZE + TILE_SIZE / 2,
                                    r * TILE_SIZE + TILE_SIZE / 2 + uiOffset));
      }
    }
  }
  return orbs;
}


inline void drawOrbs(sf::RenderWindow &window,
                     const std::vector<sf::Vector2f> &orbs) {
  sf::CircleShape o(3.f);
  o.setFillColor(sf::Color::White);
  o.setOrigin({3, 3});
  for (auto &op : orbs) {
    o.setPosition(op);
    window.draw(o);
  }
}


inline void drawScore(sf::RenderWindow &window, const sf::Font &font,
                      int score, int lives = -1) {
  sf::Text sc(font, "Score: " + std::to_string(score), 24);
  sc.setFillColor(sf::Color::White);
  sc.setPosition({10, 5});
  window.draw(sc);
  
  // Draw lives if provided
  if (lives >= 0) {
    sf::Text livesText(font, "Lives: " + std::to_string(lives), 24);
    livesText.setFillColor(sf::Color::Yellow);
    livesText.setPosition({static_cast<float>(window.getSize().x) - 150.f, 5.f});
    window.draw(livesText);
  }
}


inline bool collectOrbs(std::vector<sf::Vector2f> &orbs,
                        const sf::Vector2f &pacPos, int &score) {
  size_t before = orbs.size();
  orbs.erase(std::remove_if(orbs.begin(), orbs.end(),
                            [&](const sf::Vector2f &o) {
                              if (calcDist(pacPos, o) < TILE_SIZE / 2) {
                                score++;
                                return true;
                              }
                              return false;
                            }),
             orbs.end());
  return orbs.size() < before;
}
