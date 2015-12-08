#include "GameManager.hpp"
#include <QTimer>
#include <iostream>




GameManager::GameManager()
{ }

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();
}

void GameManager::newKinectData(const UINT16 *data, int w, int h)
{

}

void GameManager::keyPress(const int key)
{

}


void GameManager::initialize()
{
    _gameTimer = new QTimer(this);
    connect(_gameTimer, SIGNAL(timeout()), this, SLOT(updateGame()));
    _gameTimer->start(40);

    
}

void GameManager::updateGame()
{ 
}
