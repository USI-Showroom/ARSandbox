#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>
#include <QGLWidget>
#include <QMediaPlaylist>

#include <time.h>

#include "Point3.hpp"

#include "grid.hpp"

#define PI 3.1415926535897932384626433832795


static const int imgWidth=512;
static const int imgHeight=424;
#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

static int nDrops=1;

GameManager::GameManager()
: _image(imgWidth*scaling, imgHeight*scaling, QImage::Format_ARGB32)
{
    srand (time(NULL));
    _playing=false;
    _image.fill(QColor(0,0,0,0));
}

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();

    for(size_t i=0;i<_drops.size();++i)
        delete _drops[i];
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
			_drops[i] = new Drop(0.4, 0.7);

        _gameTimer->start(100);
	}
    else
    {
        for(size_t i=0;i<_drops.size();++i)
            delete _drops[i];
        
        _drops.clear();
        _gameTimer->stop();
        _image.fill(QColor(0, 0, 0, 0));
        updateTexture();   
    }
}

void GameManager::keyPress(const int key)
{ }

void GameManager::mousePress(const int x, const int y,  const int w, const int h)
{
    double normalisedX = ((double)x / (double)w);
    double normalisedY = ((double)y / (double)h);

    if (normalisedX < 0.05) normalisedX = 0.05;
    if (normalisedY < 0.05) normalisedY = 0.05;

    if (normalisedX > 0.95) normalisedX = 0.95;
    if (normalisedY > 0.95) normalisedY = 0.95;

    std::cout << "mouse press: " << normalisedX << " " << normalisedY << std::endl;

    _drops[0]->setPosition(normalisedX, normalisedY);
    _drops.at(0)->setLife(1);
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

    for(size_t i=0;i<_drops.size();++i)
    {
        Drop &a=*_drops[i];

        if(a.alive())
        {
            a.update(_mapping);
            
            Point2d p=_mapping.fromParameterization(a.position());
            p.y() = imgHeight - p.y();

            if (p.y() == imgHeight)
                --p.y();
            assert(p.y() >= 0);
            assert(p.y() < imgHeight);

            p*=scaling;

            painter.setRenderHint(QPainter::Antialiasing, true);
            
            QPen pen(Qt::red, 2);
            painter.setPen(pen);
            
            QBrush brush(Qt::red);
            painter.setBrush(brush);

            painter.drawEllipse(QPointF(p.x(), p.y()), 5, 5);

        }
    }

    painter.end();
    updateTexture();
}
