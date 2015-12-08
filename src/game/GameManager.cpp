#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>

GameManager::GameManager()
: _image(512,424,QImage::Format_ARGB32)
{ 
    _image.fill(QColor(0,0,0,0));


    //sample
    QPainter paint;
    paint.begin(&_image);
    paint.fillRect(10,10,50,50,QColor(255,0,0));
    paint.end();    
}

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
    _gameTimer->start(100);


}

void GameManager::updateGame()
{ 
    emit gameTextureChanged(_image);
}
