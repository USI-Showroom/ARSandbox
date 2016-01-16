#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>

#include "Point3.hpp"

static int w=512;
static int h=424;

GameManager::GameManager()
: _image(w,h,QImage::Format_ARGB32), _position(w/2,h/2)
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
    _mapping.setData(data);
}

void GameManager::toggleSetupMode(const bool isSetup, const int minH, const int maxH, const UnitSquareMapping &mapping)
{
    _minH=minH;
    _maxH=maxH;

    _playing=!isSetup;

    _mapping=mapping;

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
        case Qt::Key_A:
        {
            _position.x() -= step;
            break;
        }
        case Qt::Key_D:
        {
            _position.x() += step;
            break;
        }
        case Qt::Key_S:
        {
            _position.y() += step;
            break;
        }
        case Qt::Key_W:
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
#ifndef NO_KINECT
	//sample
	/*_image.fill(QColor(0, 0, 0, 0));
	QPainter painter;
	painter.begin(&_image);
	painter.setBrush(QColor(255, 0, 255, 255));
	painter.drawEllipse(QPoint(_position.x(), _position.y()), 2, 2);
	painter.end();

	updateTexture();*/
#endif
}
