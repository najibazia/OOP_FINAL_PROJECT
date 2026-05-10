


#include "Map.h"
#include <iostream>

// ── Level 1 layout — straight path ───────────────────
// Enemies enter top-left, go down col 0, turn right along row 4
static const int LAYOUT1[10][16] =
{
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

// ── Level 2 layout — winding S-curve path ────────────
// Enemies enter top-left, wind through the map in an S-shape
//
//  ↓ col0 rows 0-2
//  → row2 cols 0-8
//  ↓ col8 rows 2-7
//  → row7 cols 8-15  (exit right — tower at col13 row7)
//
static const int LAYOUT2[10][16] =
{
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

static const int* getLayout(int level)
{
    return (level == 2)
        ? &LAYOUT2[0][0]
        : &LAYOUT1[0][0];
}

static bool isPathCell(int c, int r, int level)
{
    if (r < 0 || r >= 10 || c < 0 || c >= 16) return true;
    const int* L = getLayout(level);
    return L[r * 16 + c] == 1;
}

static bool nearPathCell(int c, int r, int level)
{
    for (int dr = -1; dr <= 1; dr++)
        for (int dc = -1; dc <= 1; dc++)
            if (isPathCell(c+dc, r+dr, level)) return true;
    return false;
}

Map::Map(int cols, int rows, float tileSize, int level)
    : cols(cols), rows(rows), tileSize(tileSize), level(level)
{
    if (!grassTexture.loadFromFile("assets/grass.png"))
        std::cerr << "Warning: could not load assets/grass.png\n";
    if (!pathTexture.loadFromFile("assets/path.png"))
        std::cerr << "Warning: could not load assets/path.png\n";
    if (!blockedTexture.loadFromFile("assets/blocked.png"))
        std::cerr << "Warning: could not load assets/blocked.png\n";

    grassTexture.setRepeated(true);
    pathTexture.setRepeated(true);

    buildMap();
    buildPath();
    buildDecorations();
}

void Map::buildMap()
{
    const int* L = getLayout(level);
    grid.reserve(cols * rows);
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
        {
            TileType t = (L[r*16+c] == 1) ? TileType::PATH : TileType::GRASS;
            grid.emplace_back(t, c*tileSize, r*tileSize, tileSize,
                              grassTexture, pathTexture, blockedTexture);
        }
}

void Map::buildPath()
{
    if (level == 1)
    {
        // down col 0 rows 0-4, then right row 4 cols 0-12
        for (int r = 0; r <= 4; r++)
            path.push_back(getTile(0, r).getCenter());
        for (int c = 1; c <= 12; c++)
            path.push_back(getTile(c, 4).getCenter());
    }
    else // level 2 — S-curve
    {
        // down col 0 rows 0-2
        for (int r = 0; r <= 2; r++)
            path.push_back(getTile(0, r).getCenter());
        // right row 2 cols 1-8
        for (int c = 1; c <= 8; c++)
            path.push_back(getTile(c, 2).getCenter());
        // down col 8 rows 3-7
        for (int r = 3; r <= 7; r++)
            path.push_back(getTile(8, r).getCenter());
        // right row 7 cols 9-12
        for (int c = 9; c <= 12; c++)
            path.push_back(getTile(c, 7).getCenter());
    }
}

void Map::placeDecor(const std::string& file, float x, float y, float scale)
{
    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromFile(file))
    {
        std::cerr << "Warning: could not load " << file << "\n";
        return;
    }

    Decor d;
    d.texture = std::move(tex);
    d.sprite.setTexture(*d.texture);
    d.sprite.setScale(scale * tileSize / 64.f, scale * tileSize / 64.f);
    d.sprite.setPosition(x, y);
    decorations.push_back(std::move(d));
}

void Map::buildDecorations()
{
    decorations.reserve(200);
    float h = tileSize / 2.f;

    // trees top border
    for (int c = 1; c < cols; c += 2)
        if (!isPathCell(c, 0, level))
            placeDecor("assets/tree.png",
                       c*tileSize+h*0.3f, 0*tileSize+h*0.1f, 0.85f);
    // trees bottom border
    for (int c = 1; c < cols; c += 2)
        if (!isPathCell(c, 9, level))
            placeDecor("assets/tree.png",
                       c*tileSize+h*0.3f, 9*tileSize+h*0.1f, 0.85f);

    // rocks and flowers scattered on non-path tiles
    for (int r = 1; r < rows-1; r++)
        for (int c = 1; c < cols-1; c++)
        {
            if (isPathCell(c, r, level)) continue;
            if (nearPathCell(c, r, level)) continue;
            if ((c + r) % 4 == 0)
                placeDecor("assets/rock.png",
                           c*tileSize+h*0.4f, r*tileSize+h*0.4f, 0.55f);
            else if ((c + r) % 6 == 2)
                placeDecor((c%2==0) ? "assets/flower.png"
                                    : "assets/flower2.png",
                           c*tileSize+h*0.5f, r*tileSize+h*0.5f, 0.50f);
            else if ((c + r) % 7 == 0)
                placeDecor("assets/bush.png",
                           c*tileSize+h*0.2f, r*tileSize+h*0.2f, 0.70f);
        }

    std::cout << "Map level " << level << ": placed "
              << decorations.size() << " decorations.\n";
}

void Map::draw(sf::RenderWindow& window)
{
    for (auto& tile : grid)
        tile.draw(window);
    for (auto& d : decorations)
        window.draw(d.sprite);
}

Tile& Map::getTile(int col, int row)
{
    return grid[row * cols + col];
}

sf::Vector2i Map::pixelToGrid(sf::Vector2f pixelPos) const
{
    return { (int)(pixelPos.x/tileSize), (int)(pixelPos.y/tileSize) };
}

const std::vector<sf::Vector2f>& Map::getPath() const { return path; }
int   Map::getCols()     const { return cols; }
int   Map::getRows()     const { return rows; }
float Map::getTileSize() const { return tileSize; }