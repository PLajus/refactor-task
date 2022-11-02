#include "Game.h"

// constructor
Game::Game(IGameRules *gameRules) : gameRules(gameRules)
{
    create_window();

    playerFactory = new PlayerFactory();
    p = playerFactory->create();
    p->setPosition(sf::Vector2f(320.f, 320.f));

    playerDirectionCalculator = new EntityDirectionCalculator();
    cursorDirectionCalculator = new CursorDirectionCalculator();

    view.setSize(1280, 720);
}


// destructor
Game::~Game()
{
    delete p;

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
    window = new sf::RenderWindow(sf::VideoMode(1280, 720), "2D shooter", sf::Style::Default);
    window->setFramerateLimit(120);
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

// this class updates the game
void Game::update()
{
    gameRules->updateBullets(p->getBullets());
    p->update(cursorDirectionCalculator->calculateDirection(window, p));

    gameRules->spawnEnemy(enemies);

    for (auto enemy = enemies.begin(); enemy != enemies.end();)
    {
        (*enemy)->update(playerDirectionCalculator->calculateDirection(window, p, (*enemy)));
        gameRules->updateBullets((*enemy)->getBullets());

        if (gameRules->wasKilled(p, (*enemy)->getBullets()))
            window->close();

        if (gameRules->wasKilled((*enemy), p->getBullets())){
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

    view.setCenter(p->getPosition());

    window->setView(view);

    std::vector<Entity *> entities(combineEntities());
    for (int i = 0; i < entities.size(); i++)
    {
        entities[i]->draw(*window);
        for (auto bullet : entities[i]->getBullets())
            bullet->draw(*window);
    }

    window->display();
}

void Game::handleEvents(sf::Event event)
{
    sf::Event windowEvent;
    while (window->pollEvent(windowEvent))
    {
        if (windowEvent.type == sf::Event::Closed)
            window->close();
        if (windowEvent.type == sf::Event::KeyPressed && windowEvent.key.code == sf::Keyboard::Escape)
            window->close();
    }
}

std::vector<Entity *> Game::combineEntities()
{
    std::vector<Entity *> entities(enemies.begin(), enemies.end());
    entities.insert(entities.end(), p);
    return entities;
}