#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>
#include <QGLWidget>
#include <QMediaPlaylist>

#include <time.h>

#include "Point3.hpp"
#include "util.hpp"

#define PI 3.1415926535897932384626433832795

#define RADIUS 0.0045

static const int imgWidth=512;
static const int imgHeight=424;
#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

static int nDrops=720;

GameManager::GameManager()
: _image(imgWidth*scaling, imgHeight*scaling, QImage::Format_ARGB32),
  _simulation(new Simulation(imgWidth, imgHeight, _mapping))
{
    _playing=false;
    _image.fill(QColor(0,0,0,0));
}

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();

    for(size_t i=0;i<_drops.size();++i)
        delete _drops[i];

    delete _simulation;
}

void GameManager::updateTexture()
{
    emit gameTextureChanged(_image);
}

void GameManager::newKinectData(const UINT16 *data, int w, int h)
{
    if(!_playing) return;

    if (w != 512 && h != 424) return;

    _mapping.setData(data,_minH,_maxH);

}

void GameManager::toggleSetupMode(const bool isSetup, const int minH,
                                  const int maxH, const int gameType,
                                  const UnitSquareMapping &mapping)
{
    _minH    = minH;
    _maxH    = maxH;
    _playing = !isSetup;
    _mapping = mapping;

	if (_playing)
    {
        _drops.resize(nDrops);
		for (int i = 0; i < nDrops; ++i)
        {
#ifdef DEBUG
            std::cout << "(" << i << ") new drop" << std::endl;
#endif
			_drops[i] = new Drop(0.4, 0.7);
        }

        _gameTimer->start(100);
	}
    else
    {
        for(size_t i=0;i<_drops.size();++i)
        {
            delete _drops[i];
        }
        
        _drops.clear();
        _gameTimer->stop();
        //_image.fill(QColor(0, 0, 0, 0));
        //updateTexture();   
    }
}

void GameManager::keyPress(const int key)
{ }

void GameManager::mousePress(const int x, const int y,  const int w, const int h)
{
    double normalisedX = ((double)x / (double)w);
    double normalisedY = ((double)y / (double)h);

    util::clamp(normalisedX, 0.05, 0.95);
    util::clamp(normalisedY, 0.05, 0.95);

#ifdef DEBUG
    std::cout << "mouse press: " << normalisedX << " " << normalisedY << std::endl;
#endif

    for(size_t i = 0; i <_drops.size(); ++i)
    {
        Drop& theDrop = *_drops[i];
        theDrop.setPosition(normalisedX + RADIUS * cos(i), normalisedY + RADIUS * sin(i));
#ifdef DEBUG
        std::cout << "new drop position: " << theDrop.position() << std::endl;
#endif
        theDrop.setAlive();
    }
}

void GameManager::mouseMove(const int x, const int y,  const int w, const int h)
{ }

void GameManager::mouseRelease(const int x, const int y,  const int w, const int h)
{ }

void GameManager::initialize()
{
    _gameTimer = new QTimer(this);
    connect(_gameTimer, SIGNAL(timeout()), this, SLOT(updateGame()));

    updateTexture();
}

void GameManager::updateGame()
{	
    QPainter painter;
    painter.begin(&_image);

    for(size_t i = 0; i <_drops.size(); ++i)
    {
        Drop& d = *_drops[i];

        if(d.alive())
        {
            d.update(_mapping);
            
            Point2d p=_mapping.fromParameterization(d.position());
            p.y() = imgHeight - p.y();

            if (p.y() == imgHeight)
                --p.y();
            assert(p.y() >= 0);
            assert(p.y() < imgHeight);

            p*=scaling;

            painter.setRenderHint(QPainter::Antialiasing, true);
            
            QPen pen(Qt::blue, 2);
            painter.setPen(pen);
            
            QBrush brush(Qt::blue);
            painter.setBrush(brush);

            painter.drawEllipse(QPointF(p.x(), p.y()), 5, 5);
        }
    }

    painter.end();
    updateTexture();
}
