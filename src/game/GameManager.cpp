#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>

#include "Point3.hpp"

static int w=512;
static int h=424;
static int scaling=10;

static int nAnimals=100;

GameManager::GameManager()
: _image(w*scaling,h*scaling,QImage::Format_ARGB32), _cowTexture(":/animals/cow")
{
    srand (time(NULL));
    _playing=false;
    _image.fill(QColor(0,0,0,0));

    for(int i=0;i<nAnimals;++i)
        _animals.push_back(Animal(0.45,0.6));
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
    if(!_playing) return;

    _mapping.setData(data,_minH,_maxH);

}

void GameManager::toggleSetupMode(const bool isSetup, const int minH, const int maxH, const UnitSquareMapping &mapping)
{
    _minH=minH;
    _maxH=maxH;

    _playing=!isSetup;

    _mapping=mapping;

    if(_playing)
        _gameTimer->start(100);
    else{
        _gameTimer->stop();
        _image.fill(QColor(0, 0, 0, 0));
        updateTexture();   
    }
}

void GameManager::keyPress(const int key)
{ }


void GameManager::initialize()
{
    _gameTimer = new QTimer(this);
    connect(_gameTimer, SIGNAL(timeout()), this, SLOT(updateGame()));

    updateTexture();
}

void GameManager::updateGame()
{
    static const double imgWidth=15;
    static const double imgHeight=imgWidth/242.0*400.0;
    static const double imgOffsetW=imgWidth/2;
    static const double imgOffsetH=imgHeight/2;

    _image.fill(QColor(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&_image);
    painter.setBrush(QColor(255, 0, 255, 255));

    for(size_t i=0;i<_animals.size();++i)
    {
        Animal &a=_animals[i];
        a.think(_mapping);
        a.update();

        
        if(a.alive())
        {
            const double angle=a.angle()*180/M_PI;
            Point2d p=_mapping.fromParameterization(a.position());
            painter.translate((p.x()+imgOffsetW)*scaling,(p.y()+imgOffsetH)*scaling);
            painter.rotate(angle+90);
            painter.drawImage(QRectF(0, 0,imgWidth*scaling, imgHeight*scaling), _cowTexture);
            painter.rotate(-angle-90);
            painter.translate(-(p.x()+imgOffsetW)*scaling,-(p.y()+imgOffsetH)*scaling);
        }
    }


    painter.end();

    updateTexture();
}
