#pragma once

#include "globals.h"

inline std::vector<sf::Vector2f> spawnOrbs(float uiOffset) {
  std::vector<sf::Vector2f> orbs;
  
  for (int r = 0; r < baseMap.size(); r++) {
    for (int c = 0; c < baseMap[r].size(); c++) {
      char tile = baseMap[r][c];
      
      if (tile != '#' && tile != 'x' && tile != '-') {
        float x = c * TILE_SIZE + TILE_SIZE / 2;
        float y = r * TILE_SIZE + TILE_SIZE / 2 + uiOffset;
        orbs.push_back(sf::Vector2f(x, y));
      }
    }
  }
  return orbs;
}

inline void drawOrbs(sf::RenderWindow &window, const std::vector<sf::Vector2f> &orbs) {
  sf::CircleShape o(3.f);
  o.setFillColor(sf::Color::White);
  o.setOrigin({3.f, 3.f});
  
  for (int i = 0; i < orbs.size(); i++) {
    o.setPosition(orbs[i]);
    window.draw(o);
  }
}

inline void drawScore(sf::RenderWindow &window, const sf::Font &font, int score) {
  sf::Text sc(font, "Score: " + std::to_string(score), 24);
  sc.setFillColor(sf::Color::White);
  sc.setPosition({10.f, 5.f});
  
  window.draw(sc);
}

inline bool collectOrbs(std::vector<sf::Vector2f> &orbs, const sf::Vector2f &pacPos, int &score) {
  bool ateOrb = false;
  
  for (int i = 0; i < orbs.size(); i++) {
    if (calcDist(pacPos, orbs[i]) < TILE_SIZE / 2) {
      score++;
      
      orbs.erase(orbs.begin() + i);
      
      i--; 
      
      ateOrb = true;
    }
  }
  
  return ateOrb;
}
