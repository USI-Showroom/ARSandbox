#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>

GameManager::GameManager()
: _image(512*2,424*2,QImage::Format_ARGB32), _position(256*2,212*2)
{ 

    _playing=false;
    _image.fill(QColor(0,0,0,0));
}

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();
}

void GameManager::updateTexture()
{
    emit gameTextureChanged(_image);
}

void GameManager::newKinectData(const UINT16 *data, int w, int h)
{

}

void GameManager::toggleSetupMode(const bool isSetup, const int minH, const int maxH)
{
    _minH=minH;
    _maxH=maxH;

    _playing=!isSetup;

    if(_playing)
        _gameTimer->start(100);
    else
        _gameTimer->stop();
}

void GameManager::keyPress(const int key)
{
    static const float step=1;

    switch(key)
    {
        case Qt::Key_W:
        {
            _position.x() -= step;
            break;
        }
        case Qt::Key_S:
        {
            _position.x() += step;
            break;
        }
        case Qt::Key_A:
        {
            _position.y() += step;
            break;
        }
        case Qt::Key_D:
        {
            _position.y() -= step;
            break;
        }
    }
}


void GameManager::initialize()
{
    _gameTimer = new QTimer(this);
    connect(_gameTimer, SIGNAL(timeout()), this, SLOT(updateGame()));

    updateTexture();
}

void GameManager::updateGame()
{ 
        //sample
   /* _image.fill(QColor(0,0,0,0));
    QPainter painter;
    painter.begin(&_image);
    painter.setBrush(QColor(255,0,255,255));
    painter.drawEllipse(QPoint(_position.x(),_position.y()),10,10);
    painter.end();  

    updateTexture();*/
}
