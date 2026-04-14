#pragma once

#include "ghost_direction_and_visual_p1.h"

inline void drawGhost(sf::RenderWindow &window, sf::Vector2f pos,
                      sf::Color color, Direction dir,
                      int mood, float animTime, float tileSize)
{
    float radius = tileSize / 2.0f * 0.8f;

    sf::CircleShape dome(radius);
    dome.setOrigin({radius, radius});
    dome.setPosition(pos);
    dome.setFillColor(color);
    window.draw(dome);

    sf::RectangleShape body({radius * 2, radius});
    body.setOrigin({radius, 0});
    body.setPosition(pos);
    body.setFillColor(color);
    window.draw(body);

    float offset = std::sin(animTime * 15.0f) * 1.5f;
    sf::VertexArray tri(sf::PrimitiveType::Triangles, 9);
    float w = (radius * 2) / 3.0f;
    for (int i = 0; i < 3; ++i) {
        float sx = -radius + i * w;
        tri[i*3+0] = sf::Vertex{pos + sf::Vector2f(sx,           radius),                                    color};
        tri[i*3+1] = sf::Vertex{pos + sf::Vector2f(sx + w/2,     radius + 3 + ((i%2==0) ? offset : -offset)), color};
        tri[i*3+2] = sf::Vertex{pos + sf::Vector2f(sx + w,       radius),                                    color};
    }
    window.draw(tri);

    sf::Vector2f lk(0, 0);
    if (dir == Direction::Left)  lk.x = -1.5f;
    if (dir == Direction::Right) lk.x =  1.5f;
    if (dir == Direction::Up)    lk.y = -1.5f;
    if (dir == Direction::Down)  lk.y =  1.5f;

    sf::CircleShape eye(3.0f);
    eye.setFillColor(sf::Color::White);
    eye.setOrigin({1.5f, 1.5f});

    sf::CircleShape pupil(1.5f);
    pupil.setFillColor((mood == 1) ? sf::Color::Red : sf::Color::Blue);
    pupil.setOrigin({0.75f, 0.75f});

    for (int side = -1; side <= 1; side += 2) {
        eye.setPosition(pos + sf::Vector2f(side * 3.5f, -2.0f) + lk);
        window.draw(eye);
        pupil.setPosition(eye.getPosition() + lk);
        window.draw(pupil);
    }

    if (mood == 1) {
        sf::RectangleShape brow({5.0f, 1.5f});
        brow.setFillColor(sf::Color::Black);
        brow.setOrigin({2.5f, 0.75f});
        brow.setPosition(pos + sf::Vector2f(-3.5f, -5.5f));
        brow.setRotation(sf::degrees(25));
        window.draw(brow);
        brow.setPosition(pos + sf::Vector2f(3.5f, -5.5f));
        brow.setRotation(sf::degrees(-25));
        window.draw(brow);
    }
    else if (mood == 2) {
        sf::ConvexShape horn(3);
        horn.setPoint(0, {0, 0});
        horn.setPoint(1, {2, 0});
        horn.setPoint(2, {1, -4});
        horn.setFillColor(sf::Color::Red);
        horn.setPosition(pos + sf::Vector2f(-5, -8));
        window.draw(horn);
        horn.setPosition(pos + sf::Vector2f(3, -8));
        window.draw(horn);
    }
}

inline void drawConfusedStars(sf::RenderWindow &window,
                               sf::Vector2f ghostPos, float animTime)
{
    sf::Vector2f center = ghostPos + sf::Vector2f(0, -15.0f);
    float orbitR = 8.0f, sr = 2.8f;

    for (int i = 0; i < 3; ++i) {
        float angle = animTime * 5.0f + i * (2 * 3.14159f / 3.0f);
        sf::Vector2f sp = center + sf::Vector2f(
            std::cos(angle) * orbitR,
            std::sin(angle) * orbitR * 0.45f
        );

        sf::VertexArray star(sf::PrimitiveType::Triangles, 24);
        sf::Color sc(255, 230, 0);
        for (int j = 0; j < 8; ++j) {
            float a1 = j     * 2 * 3.14159f / 8.0f;
            float a2 = (j+1) * 2 * 3.14159f / 8.0f;
            float r1 = (j   % 2 == 0) ? sr : sr * 0.4f;
            float r2 = ((j+1)%2 == 0) ? sr : sr * 0.4f;
            star[j*3+0] = sf::Vertex{sp, sc};
            star[j*3+1] = sf::Vertex{sp + sf::Vector2f(std::cos(a1)*r1, std::sin(a1)*r1), sc};
            star[j*3+2] = sf::Vertex{sp + sf::Vector2f(std::cos(a2)*r2, std::sin(a2)*r2), sc};
        }
        window.draw(star);
    }
}