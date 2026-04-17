#pragma once
#include "ghosts_com.h"

class PredictorGhost : public Ghost {
public:
    PredictorGhost(sf::Color c) : Ghost(c) { ghostID = 1; }

protected:
    sf::Vector2f getSpecificTargetTile(sf::Vector2f pacWorldPos,
                                       sf::Vector2f pacTilePos,
                                       Direction pacDir,
                                       sf::Vector2f scatterTile,
                                       float tileSize) override {
        sf::Vector2f dir = getDirectionVector(pacDir);
        sf::Vector2f target;
        target.x = pacTilePos.x + dir.x * 4;
        target.y = pacTilePos.y + dir.y * 4;
        return target;
    }
};

class AmbusherGhost : public Ghost {
public:
    AmbusherGhost(sf::Color c) : Ghost(c) { ghostID = 3; }

    float getSpeedMultiplier() const override {
        return 1.2f;
    }

protected:
    sf::Vector2f getSpecificTargetTile(sf::Vector2f pacWorldPos,
                                       sf::Vector2f pacTilePos,
                                       Direction pacDir,
                                       sf::Vector2f scatterTile,
                                       float tileSize) override {
        sf::Vector2f dir = getDirectionVector(pacDir);
        sf::Vector2f target;
        target.x = pacTilePos.x - dir.x * 2;
        target.y = pacTilePos.y - dir.y * 2;
        return target;
    }
};