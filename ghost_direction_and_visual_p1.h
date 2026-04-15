#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

#include "globals.h"

inline void drawGhostBody(sf::RenderWindow &win, sf::Vector2f pos, sf::Color col, float t, float sz) {
    float r = (sz / 2.0f) * 0.8f;
    
    sf::CircleShape head(r);
    head.setOrigin({r, r}); head.setPosition(pos); head.setFillColor(col); win.draw(head);
    
    sf::RectangleShape rect({r * 2, r});
    rect.setOrigin({r, 0}); rect.setPosition(pos); rect.setFillColor(col); win.draw(rect);
   
    float move = std::sin(t * 15.0f) * 1.5f, w = (r * 2) / 3.0f;
    sf::VertexArray tri(sf::PrimitiveType::Triangles, 9);
    for (int i = 0; i < 3; i++) {
        float sX = -r + i * w, offY = (i % 2 == 0) ? move : -move;
        tri[i * 3] = sf::Vertex{pos + sf::Vector2f(sX, r), col};
        tri[i * 3 + 1] = sf::Vertex{pos + sf::Vector2f(sX + w / 2, r + 3 + offY), col};
        tri[i * 3 + 2] = sf::Vertex{pos + sf::Vector2f(sX + w, r), col};
    }
    win.draw(tri);
}
