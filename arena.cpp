#include "globals.h"

void drawPortal(sf::RenderWindow &window, sf::Vector2f center, float animTime)
{

  float pulse = 0.85f + 0.15f * std::sin(animTime * 4.0f);
  struct RingDef
  {
    float radius, thickness, speed;
    sf::Color cA, cB;
  };
  RingDef rings[] = {
      {13.0f * pulse, 3.5f, 1.0f, sf::Color(0, 220, 255),
       sf::Color(180, 0, 255)},
      {9.0f * pulse, 2.5f, -1.5f, sf::Color(180, 0, 255),
       sf::Color(0, 255, 180)},
      {5.0f * pulse, 2.0f, 2.5f, sf::Color(0, 255, 180),
       sf::Color(0, 180, 255)},
  };
  for (auto &r : rings)
  {

    float t = (std::sin(animTime * r.speed * 3.0f) + 1.0f) / 2.0f;

    sf::Color col((uint8_t)(r.cA.r * (1 - t) + r.cB.r * t),
                  (uint8_t)(r.cA.g * (1 - t) + r.cB.g * t),
                  (uint8_t)(r.cA.b * (1 - t) + r.cB.b * t));

    window.draw(
        createThickArc(center, r.radius, r.thickness, 0.0f, 360.0f, col));
  }

  float dotR = 3.0f * pulse;
  sf::CircleShape dot(dotR);
  dot.setOrigin(sf::Vector2f(dotR, dotR));
  dot.setPosition(center);
  float br = (std::sin(animTime * 6.0f) + 1.0f) / 2.0f;
  dot.setFillColor(
      sf::Color((uint8_t)(100 + 155 * br), (uint8_t)(200 + 55 * br), 255));
  window.draw(dot);
}

void drawArena(sf::RenderWindow &window, float uiOffset, float animTime)
{
  unsigned mapW = (unsigned)baseMap[0].size();
  unsigned mapH = (unsigned)baseMap.size();
  sf::Color wallColor(33, 33, 255); 

  for (unsigned row = 0; row < mapH; ++row)
  {
    for (unsigned col = 0; col < mapW; ++col)
    {
      char tile = baseMap[row][col];
      float xp = col * TILE_SIZE;
      float yp = row * TILE_SIZE + uiOffset;

      if (tile == '#')
      {
        float lt = 2.f, cr = 8.f; 

        bool oT = (row > 0 && baseMap[row - 1][col] != '#'); 
        bool oB =
            (row < mapH - 1 && baseMap[row + 1][col] != '#'); 
        bool oL = (col > 0 && baseMap[row][col - 1] != '#');  
        bool oR =
            (col < mapW - 1 && baseMap[row][col + 1] != '#'); 

        if (oT)
        {
          sf::RectangleShape l(
              sf::Vector2f(TILE_SIZE - (oL ? cr : 0.f) - (oR ? cr : 0.f), lt));
          l.setPosition(sf::Vector2f(xp + (oL ? cr : 0.f), yp));
          l.setFillColor(wallColor);
          window.draw(l);
        }
        if (oB)
        {
          sf::RectangleShape l(
              sf::Vector2f(TILE_SIZE - (oL ? cr : 0.f) - (oR ? cr : 0.f), lt));
          l.setPosition(
              sf::Vector2f(xp + (oL ? cr : 0.f), yp + TILE_SIZE - lt));
          l.setFillColor(wallColor);
          window.draw(l);
        }
        if (oL)
        {
          sf::RectangleShape l(
              sf::Vector2f(lt, TILE_SIZE - (oT ? cr : 0.f) - (oB ? cr : 0.f)));
          l.setPosition(sf::Vector2f(xp, yp + (oT ? cr : 0.f)));
          l.setFillColor(wallColor);
          window.draw(l);
        }
        if (oR)
        {
          sf::RectangleShape l(
              sf::Vector2f(lt, TILE_SIZE - (oT ? cr : 0.f) - (oB ? cr : 0.f)));
          l.setPosition(
              sf::Vector2f(xp + TILE_SIZE - lt, yp + (oT ? cr : 0.f)));
          l.setFillColor(wallColor);
          window.draw(l);
        }
        if (oT && oL)
          window.draw(createThickArc(sf::Vector2f(xp + cr, yp + cr), cr, lt,
                                     180, 270, wallColor));
        if (oT && oR)
          window.draw(createThickArc(sf::Vector2f(xp + TILE_SIZE - cr, yp + cr),
                                     cr, lt, 270, 360, wallColor));
        if (oB && oL)
          window.draw(createThickArc(sf::Vector2f(xp + cr, yp + TILE_SIZE - cr),
                                     cr, lt, 90, 180, wallColor));
        if (oB && oR)
          window.draw(createThickArc(
              sf::Vector2f(xp + TILE_SIZE - cr, yp + TILE_SIZE - cr), cr, lt, 0,
              90, wallColor));
      }
      else if (tile == '-')
      {
        sf::RectangleShape d(sf::Vector2f(TILE_SIZE, TILE_SIZE / 4.f));
        d.setPosition(sf::Vector2f(xp, yp + TILE_SIZE / 2.f));
        d.setFillColor(sf::Color(255, 182, 255)); 
        window.draw(d);
      }
    }
  }

  float py = 14 * TILE_SIZE + TILE_SIZE / 2 + uiOffset;
  drawPortal(window, sf::Vector2f(TILE_SIZE / 2.f, py), animTime);
  drawPortal(window, sf::Vector2f((mapW - 1) * TILE_SIZE + TILE_SIZE / 2.f, py),
             animTime);
}