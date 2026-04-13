#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

enum class Direction { None, Up, Down, Left, Right };

inline sf::Vector2f getDirectionVector(Direction d) {
    if (d == Direction::Up) return sf::Vector2f(0, -1);
    else if (d == Direction::Down) return sf::Vector2f(0, 1);
    else if (d == Direction::Left) return sf::Vector2f(-1, 0);
    else if (d == Direction::Right) return sf::Vector2f(1, 0);
    return sf::Vector2f(0, 0);
}

inline float calcDist(sf::Vector2f p1, sf::Vector2f p2) {
    float dx = p1.x - p2.x, dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}

inline void drawGhostBody(sf::RenderWindow &win, sf::Vector2f pos, sf::Color col, float t, float sz) {
    float r = (sz / 2.0f) * 0.8f;
    
    sf::CircleShape head(r);
    head.setOrigin(r, r); head.setPosition(pos); head.setFillColor(col); win.draw(head);
    
    sf::RectangleShape rect(sf::Vector2f(r * 2, r));
    rect.setOrigin(r, 0); rect.setPosition(pos); rect.setFillColor(col); win.draw(rect);
   
    float move = std::sin(t * 15.0f) * 1.5f, w = (r * 2) / 3.0f;
    sf::VertexArray tri(sf::Triangles, 9);
    for (int i = 0; i < 3; i++) {
        float sX = -r + i * w, offY = (i % 2 == 0) ? move : -move;
        tri[i * 3] = sf::Vertex(pos + sf::Vector2f(sX, r), col);
        tri[i * 3 + 1] = sf::Vertex(pos + sf::Vector2f(sX + w / 2, r + 3 + offY), col);
        tri[i * 3 + 2] = sf::Vertex(pos + sf::Vector2f(sX + w, r), col);
    }
    win.draw(tri);
}
