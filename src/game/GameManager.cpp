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

static const int simulationSize = 38;

GameManager::GameManager()
: _image(imgWidth*scaling, imgHeight*scaling, QImage::Format_ARGB32),
  _simulation(new Simulation(simulationSize, simulationSize, _mapping)),
  _grid(new Grid(simulationSize, &_mapping))
{
    _playing=false;
    _image.fill(QColor(0,0,0,0));
    _simulation->setGrid(_grid);

    int gridIndex = _grid->getCellIndex( 0.56, 0.67 );
    _simulation->addWaterSource( gridIndex, 10.0 );
}

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();

    delete _simulation;
    delete _grid;
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
        _gameTimer->start(100);
	}
    else
    {
        _gameTimer->stop();
        _image.fill(QColor(0, 0, 0, 0));
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

    // convert to grid cell
    int gridIndex = _grid->getCellIndex(normalisedX, normalisedY);
#ifdef DEBUG
    std::cout << "grid index: " << gridIndex << std::endl;
#endif
    double amount = 10.0;
    _simulation->addWaterSource( gridIndex, amount );
    std::cout << " Water amount " << amount << " added at cell " << gridIndex << std::endl;
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
    _image.fill(QColor(0, 0, 0, 0));

    QPainter painter;
    painter.begin(&_image);

    _simulation->update(0.01);
    emit rangeChanged((float)_simulation->_minW, (float)_simulation->_maxW,
                      (float)_simulation->_minS, (float)_simulation->_maxS);
    _simulation->draw(painter, _mapping);

    painter.end();
    updateTexture();
}
