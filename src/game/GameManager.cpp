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

double GameManager::getHeight(const UINT16 *data, const int x, const int y)
{
    return double(data[x*h + y]-_minH)/(_maxH-_minH);
}

void GameManager::newKinectData(const UINT16 *data, int w, int h)
{
    // const double currentH=getHeight(data,_position.x(),_position.y());

    // Point3d d0(-1,-1,getHeight(data,_position.x()-1,_position.y()-1)-currentH);
    // Point3d d1(-1,+1,getHeight(data,_position.x()-1,_position.y()+1)-currentH);
    // Point3d d2(+1,-1,getHeight(data,_position.x()+1,_position.y()-1)-currentH);
    // Point3d d3(+1,+1,getHeight(data,_position.x()+1,_position.y()+1)-currentH);

    // d0.normalize(); d1.normalize(); d2.normalize(); d3.normalize();

    // const Point3d n=(d0+d1+d2+d3).normalized();
    // std::cout<<n<<std::endl;
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
        case Qt::Key_W:
        {
            _position.y() += step;
            break;
        }
        case Qt::Key_S:
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
    // _image.fill(QColor(0,0,0,0));
    // QPainter painter;
    // painter.begin(&_image);
    // painter.setBrush(QColor(255,0,255,255));
    // painter.drawEllipse(QPoint(_position.x(),_position.y()),10,10);
    // painter.end();  

    updateTexture();
}
