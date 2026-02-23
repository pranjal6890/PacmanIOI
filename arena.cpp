#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

int main() {
    // 1. Define the Map (The Tile Grid)
    // '#' = Wall, '-' = Ghost Door, ' ' = Empty space
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

    // 2. Set up dimensions
    const float tileSize = 20.0f; // Each block is 20x20 pixels
    unsigned int mapWidth = levelMap[0].size();
    unsigned int mapHeight = levelMap.size();

    // 3. Create the SFML 3.0 Window
    // We calculate window size based on our map size and tile size
    sf::RenderWindow window(sf::VideoMode({mapWidth * (unsigned int)tileSize, 
                                           mapHeight * (unsigned int)tileSize}), 
                            "Pac-Man Arena");

    // Define arcade neon colors
    sf::Color wallColor(33, 33, 255);    // Arcade Blue

    // 4. Main Game Loop
    while (window.isOpen()) {
        
        // --- EVENT HANDLING (SFML 3.0 style) ---
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // If the user clicks the 'X' to close the window
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- RENDERING ---
        window.clear(sf::Color::Black); // Clear previous frame with black background

        // Loop through our rows (Y axis)
        for (unsigned int row = 0; row < mapHeight; ++row) {
            // Loop through our columns (X axis)
            for (unsigned int col = 0; col < mapWidth; ++col) {
                
                char tile = levelMap[row][col];
                
                // Calculate where to draw this specific tile on the screen
                float xPos = col * tileSize;
                float yPos = row * tileSize;

                if (tile == '#') {
                    // Draw a Wall
                    sf::RectangleShape wall(sf::Vector2f({tileSize, tileSize}));
                    wall.setPosition({xPos, yPos});
                    // Making it hollow with an outline gives a simple "arcade" look
                    wall.setFillColor(sf::Color::Black);
                    wall.setOutlineThickness(-2.0f); // Negative draws inward
                    wall.setOutlineColor(wallColor);
                    window.draw(wall);
                } 
                else if (tile == '-') {
                    // Ghost house door
                    sf::RectangleShape door(sf::Vector2f({tileSize, tileSize / 4.0f}));
                    door.setPosition({xPos, yPos + (tileSize / 2.0f)});
                    door.setFillColor(sf::Color(255, 182, 255)); // Pinkish door
                    window.draw(door);
                }
            }
        }

        // Display everything we just drew
        window.display();
    }

    return 0;
}