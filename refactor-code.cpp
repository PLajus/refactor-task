#include "Game.h"

Game::Game(IGameRules *gameRules) : gameRules(gameRules)
{
    playerFactory = new PlayerFactory();
    player = playerFactory->create();
    playerDirectionCalculator = new EntityDirectionCalculator();
    cursorDirectionCalculator = new CursorDirectionCalculator();

    player->setPosition(sf::Vector2f(PLAYER_START_X, PLATER_START_Y));

    createWindow();

    view.setSize(VIEW_WIDTH, VIEW_HEIGHT);
}

Game::~Game()
{
    delete player;
    for (auto *enemy : enemies)
        delete enemy;
    delete playerDirectionCalculator;
    delete cursorDirectionCalculator;
    delete gameRules;
    delete playerFactory;
    delete window;
}

void Game::createWindow()
{
    window = new sf::RenderWindow(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "2D shooter", sf::Style::Default);
    window->setFramerateLimit(FRAME_RATE_LIMIT);
    window->setVerticalSyncEnabled(false);
}

void Game::run()
{
    while (window->isOpen())
    {
        handleEvents();
        update();
        draw();
    }
}

void Game::update()
{
    gameRules->updateBullets(player->getBullets());
    player->update(cursorDirectionCalculator->calculateDirection(window, player));

    gameRules->spawnEnemy(enemies);

    checkKills()
}

void Game : checkKills()
{
    for (auto enemy = enemies.begin(); enemy != enemies.end();)
    {
        (*enemy)->update(playerDirectionCalculator->calculateDirection(window, player, (*enemy)));
        gameRules->updateBullets((*enemy)->getBullets());

        if (gameRules->wasKilled(player, (*enemy)->getBullets()))
            window->close();

        if (gameRules->wasKilled((*enemy), player->getBullets()))
        {
            (*enemy)->die();
            enemy = enemies.erase(enemy);
        }
        else
            ++enemy;
    }
}

void Game::draw()
{
    window->clear();

    view.setCenter(player->getPosition());

    window->setView(view);

    std::vector<Entity *> entities(combineEntities());
    for (auto entity : entities)
    {
        entity->draw(*window);
        for (auto bullet : entity->getBullets())
            bullet->draw(*window);
    }

    window->display();
}

void Game::handleEvents()
{
    sf::Event windowEvent;
    while (window->pollEvent(windowEvent))
    {
        if (windowEvent.type == sf::Event::Closed ||
            (sf::Event::KeyPressed && windowEvent.key.code == sf::Keyboard::Escape))
            window->close();
    }
}

std::vector<Entity *> Game::combineEntities()
{
    std::vector<Entity *> entities(enemies.begin(), enemies.end());
    entities.insert(entities.end(), player);
    return entities;
}