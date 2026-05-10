


#pragma once
#include <vector>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Tile.h"

struct Decor
{
    std::unique_ptr<sf::Texture> texture;
    sf::Sprite                   sprite;
};

class Map
{
public:
    Map(int cols, int rows, float tileSize, int level);

    void draw(sf::RenderWindow& window);

    Tile&        getTile(int col, int row);
    sf::Vector2i pixelToGrid(sf::Vector2f pixelPos) const;

    const std::vector<sf::Vector2f>& getPath() const;

    int   getCols()     const;
    int   getRows()     const;
    float getTileSize() const;

private:
    void buildMap();
    void buildPath();
    void buildDecorations();
    void placeDecor(const std::string& file,
                    float x, float y, float scale);

    int   cols, rows;
    float tileSize;
    int   level;     

    sf::Texture grassTexture;
    sf::Texture pathTexture;
    sf::Texture blockedTexture;

    std::vector<Tile>         grid;
    std::vector<sf::Vector2f> path;
    std::vector<Decor>        decorations;
};
