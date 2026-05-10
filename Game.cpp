#include "Game.h"
#include <string>
#include <iostream>
#include <cmath>

static const float TILE       = 60.f;
static const float TOWER_COL1 = 13.f, TOWER_ROW1 = 4.f;
static const float TOWER_COL2 = 12.f, TOWER_ROW2 = 7.f;
static const float WIN_W      = 960.f;
static const float WIN_H      = 600.f;

Game::Game()
    : window(sf::VideoMode((int)WIN_W, (int)WIN_H), "Tower Defense")
{
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/font.ttf"))
        std::cerr << "ERROR: could not load assets/font.ttf\n";
    else
        fontLoaded = true;

    yesButton.setSize({ 180.f, 60.f });
    yesButton.setFillColor(sf::Color(40, 160, 60));
    yesButton.setOutlineThickness(3.f);
    yesButton.setOutlineColor(sf::Color(20, 100, 30));
    yesButton.setPosition(240.f, 390.f);

    noButton.setSize({ 180.f, 60.f });
    noButton.setFillColor(sf::Color(180, 40, 40));
    noButton.setOutlineThickness(3.f);
    noButton.setOutlineColor(sf::Color(110, 20, 20));
    noButton.setPosition(540.f, 390.f);

    menuMap = std::make_unique<Map>(16, 10, TILE, 1);
    state   = GameState::MENU;
}

// ── playerShoot ───────────────────────────────────────────────────
// Fires arrow.png in whatever direction the player is facing.
// No targeting — pure directional shot controlled by the player.
void Game::playerShoot()
{
    auto arrow = player->shoot();
    if (arrow)
        projectiles.push_back(std::move(arrow));
}

void Game::loadLevel(int level)
{
    currentLevel = level;
    state        = GameState::PLAYING;
    stateTimer   = 0.f;
    spawnTimer   = 0.f;
    killCount    = 0;
    levelTimer   = 0.f;
    gameTime     = 0.f;
    enemies.clear();
    projectiles.clear();

    float tCol = (level == 1) ? TOWER_COL1 : TOWER_COL2;
    float tRow = (level == 1) ? TOWER_ROW1 : TOWER_ROW2;

    map   = std::make_unique<Map>(16, 10, TILE, level);
    tower = std::make_unique<Tower>(
                tCol * TILE + TILE / 2.f,
                tRow * TILE + TILE / 2.f);

    sf::Vector2f towerPos    = tower->getPosition();
    sf::Vector2f playerStart = { towerPos.x - 90.f, towerPos.y };

    player = std::make_unique<Player>(
        playerStart, towerPos,
        16.f * TILE, 10.f * TILE);

    buildSpawnQueue(level);
    std::cout << "=== Level " << level
              << " | need " << getConfig().enemiesToAdvance << " kills ===\n";
}

void Game::buildSpawnQueue(int level)
{
    spawnQueue = EventQueue<SpawnEvent>();
    const LevelConfig& cfg = (level == 2) ? LEVEL2 : LEVEL1;
    int count = cfg.enemiesToAdvance + 10;

    for (int i = 0; i < count; i++)
    {
        float hpMult    = (i % 5 == 4) ? 1.5f : 1.0f;
        float speedMult = (i % 5 == 4) ? 1.2f : 1.0f;
        SpawnEvent ev;
        ev.wave   = i / 5;
        ev.hp     = cfg.enemyHP    * hpMult;
        ev.speed  = cfg.enemySpeed * speedMult;
        ev.damage = cfg.enemyDamage;
        spawnQueue.push(cfg.spawnInterval * i, ev);
    }
    std::cout << "Spawn queue: " << spawnQueue.size() << " enemies\n";
}

const LevelConfig& Game::getConfig() const
{
    return (currentLevel == 2) ? LEVEL2 : LEVEL1;
}

void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (state == GameState::PLAYING)
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::K)
                playerShoot();

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
            if (event.key.code == sf::Keyboard::R)
                if (state == GameState::GAME_OVER ||
                    state == GameState::VICTORY)
                    state = GameState::MENU;
        }

        if (event.type == sf::Event::MouseButtonPressed)
            if (event.mouseButton.button == sf::Mouse::Left)
                if (state == GameState::MENU)
                    handleMenuClick({(float)event.mouseButton.x,
                                     (float)event.mouseButton.y});
    }
}

void Game::handleMenuClick(sf::Vector2f pos)
{
    if (yesButton.getGlobalBounds().contains(pos)) loadLevel(1);
    else if (noButton.getGlobalBounds().contains(pos)) window.close();
}

void Game::update(float dt)
{
    switch (state)
    {
        case GameState::MENU: break;
        case GameState::PLAYING: updatePlaying(dt); break;
        case GameState::LEVEL_UP:
            stateTimer += dt;
            if (stateTimer >= 2.5f) loadLevel(2);
            break;
        case GameState::GAME_OVER:
        case GameState::VICTORY: break;
    }
}

void Game::updatePlaying(float dt)
{
    gameTime   += dt;
    levelTimer += dt;

    // Spawn
    while (!spawnQueue.empty() && spawnQueue.top().time <= gameTime)
    {
        SpawnEvent ev = spawnQueue.pop().data;
        enemies.push_back(std::make_unique<Enemy>(
            map->getPath(), ev.hp, ev.speed, ev.damage));
    }

    for (auto& e : enemies) e->update(dt);
    for (int i = 0; i < (int)enemies.size(); i++)
        enemies[i]->separate(enemies, i);

    {
        std::vector<int> attackers;
        for (int i = 0; i < (int)enemies.size(); i++)
            if (enemies[i]->isAttacking() && !enemies[i]->isDead())
                attackers.push_back(i);
        for (int s = 0; s < (int)attackers.size(); s++)
            enemies[attackers[s]]->assignAttackSlot(
                tower->getPosition(), s, (int)attackers.size());
    }

    applyEnemyAttacks(dt);
    tower->update(dt, enemies);
    player->update(dt);
    updateProjectiles(dt);
    removeDeadEnemies();

    if (tower->isDead())
    { gameOverReason = "Your tower was destroyed!"; state = GameState::GAME_OVER; return; }

    if (levelTimer >= levelTimeLimit && killCount < getConfig().enemiesToAdvance)
    { gameOverReason = "Time's up!"; state = GameState::GAME_OVER; return; }

    if (killCount >= getConfig().enemiesToAdvance)
    {
        if (currentLevel < 2) { currentLevel++; state = GameState::LEVEL_UP; stateTimer = 0.f; }
        else state = GameState::VICTORY;
    }
}

void Game::collectProjectiles() {}

void Game::updateProjectiles(float dt)
{
    const float HIT_RADIUS = 28.f;

    for (auto& proj : projectiles)
    {
        if (proj->isExpired()) continue;
        proj->update(dt);

        sf::Vector2f pPos = proj->getPosition();
        for (auto& e : enemies)
        {
            if (e->isDead() || e->isDeathDone()) continue;
            sf::Vector2f diff = pPos - e->getPosition();
            float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
            if (dist < HIT_RADIUS)
            { e->takeDamage(proj->getDamage()); proj->expire(); break; }
        }
    }

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const std::unique_ptr<Projectile>& p){ return p->isExpired(); }),
        projectiles.end());
}

void Game::applyEnemyAttacks(float dt)
{
    for (auto& e : enemies)
        if (e->isAttacking())
            tower->takeDamage(e->getAttackDamage() * dt);
}

void Game::spawnEnemy()
{
    const LevelConfig& cfg = getConfig();
    enemies.push_back(std::make_unique<Enemy>(
        map->getPath(), cfg.enemyHP, cfg.enemySpeed, cfg.enemyDamage));
}

void Game::removeDeadEnemies()
{
    for (auto& e : enemies)
        if (e->isDead() && !e->killCounted)
        { e->killCounted = true; killCount++;
          std::cout << "Kill " << killCount << "/" << getConfig().enemiesToAdvance << "\n"; }

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e){ return e->isDeathDone(); }),
        enemies.end());
}

void Game::render()
{
    window.clear(sf::Color(20, 20, 30));
    if (state == GameState::MENU) { drawMenu(); }
    else
    {
        map->draw(window);
        tower->draw(window);
        for (auto& p : projectiles) p->draw(window);
        for (auto& e : enemies)     e->draw(window);
        player->draw(window);
        drawHUD();
        if (state == GameState::LEVEL_UP)  drawLevelUp();
        if (state == GameState::GAME_OVER) drawGameOver();
        if (state == GameState::VICTORY)   drawVictory();
    }
    window.display();
}

void Game::drawText(const std::string& str, float x, float y,
                    unsigned int size, sf::Color color)
{
    if (!fontLoaded) return;
    sf::Text t(str, font, size);
    t.setFillColor(color); t.setPosition(x, y); window.draw(t);
}

void Game::drawCentredText(const std::string& str, float y,
                           unsigned int size, sf::Color color)
{
    if (!fontLoaded) return;
    sf::Text t(str, font, size);
    t.setFillColor(color);
    t.setPosition((WIN_W - t.getLocalBounds().width) / 2.f, y);
    window.draw(t);
}

static void drawRect(sf::RenderWindow& w, float x, float y,
                     float ww, float hh, sf::Color fill,
                     sf::Color outline=sf::Color::Transparent,
                     float thick=0.f, float=0.f)
{
    sf::RectangleShape r({ww,hh});
    r.setFillColor(fill);
    if (thick>0.f){r.setOutlineThickness(thick);r.setOutlineColor(outline);}
    r.setPosition(x,y); w.draw(r);
}

void Game::drawMenu()
{
    window.clear(sf::Color(20,25,20));
    menuMap->draw(window);

    static std::unique_ptr<Tower> menuTower;
    static bool menuTowerBuilt=false;
    if(!menuTowerBuilt){menuTower=std::make_unique<Tower>(13.f*60.f+30.f,4.f*60.f+30.f);menuTowerBuilt=true;}
    menuTower->draw(window);

    static bool goblinsBuilt=false;
    static sf::Texture goblinTex;
    static std::vector<sf::Sprite> goblins;
    if(!goblinsBuilt){
        if(goblinTex.loadFromFile("assets/walk.png")){
            float py2=4.f*60.f+30.f;
            for(float gx:{1.f*60.f+30.f,3.f*60.f+30.f,5.f*60.f+30.f,7.f*60.f+30.f}){
                sf::Sprite spr(goblinTex);
                spr.setTextureRect(sf::IntRect(0,0,48,48));
                spr.setScale(2.2f,2.2f);spr.setOrigin(24.f,24.f);spr.setPosition(gx,py2);
                goblins.push_back(spr);
            }
        }
        goblinsBuilt=true;
    }
    for(auto& g:goblins) window.draw(g);

    sf::RectangleShape overlay({WIN_W,WIN_H});
    overlay.setFillColor(sf::Color(0,0,0,110));window.draw(overlay);

    float pw=400.f,ph=360.f,panX=(WIN_W-pw)/2.f,panY=105.f;
    sf::RectangleShape shadow({pw,ph});
    shadow.setFillColor(sf::Color(0,0,0,130));shadow.setPosition(panX+6,panY+6);window.draw(shadow);
    sf::RectangleShape panel({pw,ph});
    panel.setFillColor(sf::Color(4,16,6,230));panel.setOutlineThickness(2.5f);
    panel.setOutlineColor(sf::Color(38,105,20));panel.setPosition(panX,panY);window.draw(panel);
    drawRect(window,panX,panY,pw,7,sf::Color(55,130,28));
    drawRect(window,panX+18,panY+104,pw-36,1.5f,sf::Color(38,105,20,160));
    drawRect(window,panX+18,panY+212,pw-36,1.5f,sf::Color(38,105,20,160));

    auto diamond=[&](float dx,float dy){
        sf::ConvexShape d(4);
        d.setPoint(0,{dx,dy-7});d.setPoint(1,{dx+7,dy});
        d.setPoint(2,{dx,dy+7});d.setPoint(3,{dx-7,dy});
        d.setFillColor(sf::Color(200,160,18,200));window.draw(d);
    };
    diamond(panX+16,panY+16);diamond(panX+pw-16,panY+16);
    diamond(panX+16,panY+ph-16);diamond(panX+pw-16,panY+ph-16);

    drawCentredText("TOWER DEFENSE",        panY+22.f,  40,sf::Color(232,196,32));
    drawCentredText("- DEFEND THE REALM -", panY+74.f,  13,sf::Color(110,175,60));
    drawCentredText("Survive goblin waves. Protect your castle.",panY+120.f,15,sf::Color(155,210,120));
    drawCentredText("Level 1  -  Kill 15 goblins to advance",   panY+148.f,14,sf::Color(125,185,95));
    drawCentredText("Level 2  -  Kill 20 goblins to win",       panY+172.f,14,sf::Color(125,185,95));
    drawCentredText("PLAY NOW?",panY+228.f,22,sf::Color(215,215,175));

    yesButton.setPosition(panX+24.f,panY+252.f);yesButton.setSize({160.f,54.f});
    yesButton.setFillColor(sf::Color(22,100,22));yesButton.setOutlineThickness(2.f);
    yesButton.setOutlineColor(sf::Color(40,165,36));window.draw(yesButton);
    if(fontLoaded){sf::Text yes("YES",font,22);yes.setStyle(sf::Text::Bold);
        yes.setFillColor(sf::Color(185,255,175));
        yes.setPosition(yesButton.getPosition().x+(160.f-yes.getLocalBounds().width)/2.f,
                        yesButton.getPosition().y+(54.f-yes.getLocalBounds().height)/2.f-4.f);
        window.draw(yes);}

    noButton.setPosition(panX+pw-184.f,panY+252.f);noButton.setSize({160.f,54.f});
    noButton.setFillColor(sf::Color(100,18,18));noButton.setOutlineThickness(2.f);
    noButton.setOutlineColor(sf::Color(168,36,36));window.draw(noButton);
    if(fontLoaded){sf::Text no("NO",font,22);no.setStyle(sf::Text::Bold);
        no.setFillColor(sf::Color(255,185,175));
        no.setPosition(noButton.getPosition().x+(160.f-no.getLocalBounds().width)/2.f,
                       noButton.getPosition().y+(54.f-no.getLocalBounds().height)/2.f-4.f);
        window.draw(no);}

    drawCentredText("Click YES to play  |  ESC to quit",panY+330.f,13,sf::Color(72,112,50));
}

void Game::drawHUD()
{
    const LevelConfig& cfg=getConfig();
    int timeLeft=(int)(levelTimeLimit-levelTimer);
    sf::RectangleShape strip({WIN_W,34.f});
    strip.setFillColor(sf::Color(0,0,0,180));strip.setPosition(0.f,0.f);window.draw(strip);
    drawText("LVL "+std::to_string(currentLevel),8.f,8.f,16,sf::Color(240,200,30));
    drawText("Kills: "+std::to_string(killCount)+"/"+std::to_string(cfg.enemiesToAdvance),
             130.f,8.f,16,sf::Color::White);
    drawText("Tower HP: "+std::to_string((int)tower->getHP()),340.f,8.f,16,sf::Color(80,220,100));
    sf::Color tc=(timeLeft<=10)?sf::Color(255,80,80):sf::Color(200,200,200);
    drawText("Time: "+std::to_string(timeLeft)+"s",680.f,8.f,16,tc);
    drawText("Move: Arrows/WASD     K: Shoot in facing direction",
             140.f,WIN_H-22.f,13,sf::Color(140,140,140));
}

void Game::drawLevelUp()
{
    sf::RectangleShape ov({WIN_W,WIN_H});ov.setFillColor(sf::Color(0,0,0,170));window.draw(ov);
    sf::RectangleShape box({700.f,220.f});box.setFillColor(sf::Color(10,60,10,230));
    box.setOutlineThickness(3.f);box.setOutlineColor(sf::Color(240,185,30));
    box.setPosition(130.f,180.f);window.draw(box);
    drawCentredText("LEVEL 2!",195.f,58,sf::Color(240,200,30));
    drawCentredText("Well done! You survived Level 1.",290.f,22,sf::Color::White);
    drawCentredText("Enemies are faster and tougher now!",325.f,20,sf::Color(180,230,160));
    drawCentredText("Get ready...",365.f,17,sf::Color(160,160,160));
}

void Game::drawGameOver()
{
    sf::RectangleShape ov({WIN_W,WIN_H});ov.setFillColor(sf::Color(0,0,0,185));window.draw(ov);
    sf::RectangleShape box({720.f,260.f});box.setFillColor(sf::Color(60,10,10,230));
    box.setOutlineThickness(3.f);box.setOutlineColor(sf::Color(200,50,50));
    box.setPosition(120.f,160.f);window.draw(box);
    drawCentredText("GAME OVER",175.f,58,sf::Color(255,70,70));
    drawCentredText(gameOverReason,278.f,20,sf::Color::White);
    drawCentredText("Kills this run: "+std::to_string(killCount),316.f,18,sf::Color(200,200,200));
    sf::RectangleShape btn({320.f,48.f});btn.setFillColor(sf::Color(80,20,20,220));
    btn.setOutlineThickness(2.f);btn.setOutlineColor(sf::Color(200,80,80));
    btn.setPosition(320.f,368.f);window.draw(btn);
    drawCentredText("Press R  -  Back to Menu",380.f,18,sf::Color::White);
}

void Game::drawVictory()
{
    sf::RectangleShape ov({WIN_W,WIN_H});ov.setFillColor(sf::Color(0,0,0,170));window.draw(ov);
    sf::RectangleShape box({720.f,260.f});box.setFillColor(sf::Color(10,60,20,230));
    box.setOutlineThickness(3.f);box.setOutlineColor(sf::Color(50,200,80));
    box.setPosition(120.f,160.f);window.draw(box);
    drawCentredText("YOU WIN!",175.f,58,sf::Color(60,230,100));
    drawCentredText("You defended the tower through both levels!",278.f,20,sf::Color::White);
    drawCentredText("Total kills: "+std::to_string(killCount),316.f,18,sf::Color(200,200,200));
    sf::RectangleShape btn({320.f,48.f});btn.setFillColor(sf::Color(10,60,20,220));
    btn.setOutlineThickness(2.f);btn.setOutlineColor(sf::Color(50,200,80));
    btn.setPosition(320.f,368.f);window.draw(btn);
    drawCentredText("Press R  -  Back to Menu",380.f,18,sf::Color::White);
}