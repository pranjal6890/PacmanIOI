#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>
#include <cmath>


sf::VertexArray createThickArc(sf::Vector2f center, float radius, float thickness, float startAngle, float endAngle, sf::Color color) {
    const int points = 15; 
    sf::VertexArray arc(sf::PrimitiveType::TriangleStrip, points * 2);
    
    for (int i = 0; i < points; ++i) {
        
        float angle = startAngle + (endAngle - startAngle) * (i / (float)(points - 1));
        float rad = angle * 3.14159f / 180.0f;
        sf::Vector2f dir(std::cos(rad), std::sin(rad));
        
        
        arc[i * 2].position = center + dir * radius;
        arc[i * 2].color = color;
        
        
        arc[i * 2 + 1].position = center + dir * (radius - thickness);
        arc[i * 2 + 1].color = color;
    }
    return arc;
}

int main() {
    
    std::vector<std::string> levelMap = {
        "############################",
        "#            ##            #",
        "# #### ##### ## ##### #### #",
        "# #### ##### ## ##### #### #",
        "# #### ##### ## ##### #### #",
        "#                          #",
        "# #### ## ######## ## #### #",
        "# #### ## ######## ## #### #",
        "#      ##    ##    ##      #",
        "###### ##### ## ##### ######",
        "     # ##### ## ##### #     ",
        "     # ##          ## #     ",
        "     # ## ###--### ## #     ",
        "###### ## #      # ## ######",
        "          #      #          ",
        "###### ## #      # ## ######",
        "     # ## ######## ## #     ",
        "     # ##          ## #     ",
        "     # ## ######## ## #     ",
        "###### ## ######## ## ######",
        "#            ##            #",
        "# #### ##### ## ##### #### #",
        "# #### ##### ## ##### #### #",
        "#   ##                ##   #",
        "### ## ## ######## ## ## ###",
        "### ## ## ######## ## ## ###",
        "#      ##    ##    ##      #",
        "# ########## ## ########## #",
        "# ########## ## ########## #",
        "#                          #",
        "############################"
    };

    
    const float tileSize = 20.0f; 
    unsigned int mapWidth = levelMap[0].size();
    unsigned int mapHeight = levelMap.size();

    
    sf::RenderWindow window(sf::VideoMode({mapWidth * (unsigned int)tileSize, 
                                           mapHeight * (unsigned int)tileSize}), 
                            "Pac-Man Arena");

    
    sf::Color wallColor(33, 33, 255);   

   
    while (window.isOpen()) {
        
      
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

       
        window.clear(sf::Color::Black); 

        
        for (unsigned int row = 0; row < mapHeight; ++row) {
           
            for (unsigned int col = 0; col < mapWidth; ++col) {
                
                char tile = levelMap[row][col];
                
               
                float xPos = col * tileSize;
                float yPos = row * tileSize;

                if (tile == '#') {
                    float lineThickness = 2.0f;
                    float cornerRadius = 8.0f;

                    
                    bool openTop = (row > 0 && levelMap[row - 1][col] != '#');
                    bool openBottom = (row < mapHeight - 1 && levelMap[row + 1][col] != '#');
                    bool openLeft = (col > 0 && levelMap[row][col - 1] != '#');
                    bool openRight = (col < mapWidth - 1 && levelMap[row][col + 1] != '#');

                   
                    if (openTop) {
                        float startX = xPos + (openLeft ? cornerRadius : 0);
                        float width = tileSize - (openLeft ? cornerRadius : 0) - (openRight ? cornerRadius : 0);
                        sf::RectangleShape line(sf::Vector2f({width, lineThickness}));
                        line.setPosition({startX, yPos});
                        line.setFillColor(wallColor);
                        window.draw(line);
                    }
                    if (openBottom) {
                        float startX = xPos + (openLeft ? cornerRadius : 0);
                        float width = tileSize - (openLeft ? cornerRadius : 0) - (openRight ? cornerRadius : 0);
                        sf::RectangleShape line(sf::Vector2f({width, lineThickness}));
                        line.setPosition({startX, yPos + tileSize - lineThickness});
                        line.setFillColor(wallColor);
                        window.draw(line);
                    }
                    if (openLeft) {
                        float startY = yPos + (openTop ? cornerRadius : 0);
                        float height = tileSize - (openTop ? cornerRadius : 0) - (openBottom ? cornerRadius : 0);
                        sf::RectangleShape line(sf::Vector2f({lineThickness, height}));
                        line.setPosition({xPos, startY});
                        line.setFillColor(wallColor);
                        window.draw(line);
                    }
                    if (openRight) {
                        float startY = yPos + (openTop ? cornerRadius : 0);
                        float height = tileSize - (openTop ? cornerRadius : 0) - (openBottom ? cornerRadius : 0);
                        sf::RectangleShape line(sf::Vector2f({lineThickness, height}));
                        line.setPosition({xPos + tileSize - lineThickness, startY});
                        line.setFillColor(wallColor);
                        window.draw(line);
                    }

                    
                    if (openTop && openLeft) {
                        window.draw(createThickArc({xPos + cornerRadius, yPos + cornerRadius}, 
                                                    cornerRadius, lineThickness, 180.0f, 270.0f, wallColor));
                    }
                   
                    if (openTop && openRight) {
                        window.draw(createThickArc({xPos + tileSize - cornerRadius, yPos + cornerRadius}, 
                                                    cornerRadius, lineThickness, 270.0f, 360.0f, wallColor));
                    }
                    
                    if (openBottom && openLeft) {
                        window.draw(createThickArc({xPos + cornerRadius, yPos + tileSize - cornerRadius}, 
                                                    cornerRadius, lineThickness, 90.0f, 180.0f, wallColor));
                    }
                   
                    if (openBottom && openRight) {
                        window.draw(createThickArc({xPos + tileSize - cornerRadius, yPos + tileSize - cornerRadius}, 
                                                    cornerRadius, lineThickness, 0.0f, 90.0f, wallColor));
                    }
                }
                else if (tile == '-') {
                   
                    sf::RectangleShape door(sf::Vector2f({tileSize, tileSize / 4.0f}));
                    door.setPosition({xPos, yPos + (tileSize / 2.0f)});
                    door.setFillColor(sf::Color(255, 182, 255)); 
                    window.draw(door);
                }
            }
        }

        
        window.display();
    }

    return 0;
}