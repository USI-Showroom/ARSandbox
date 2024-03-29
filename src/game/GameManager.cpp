#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>
#include <QGLWidget>
#include <QMediaPlaylist>

#include <time.h>

#include "Point3.hpp"

#define PI 3.1415926535897932384626433832795


static const int imgWidth=512;
static const int imgHeight=424;
#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

static int nAnimals=40;

GameManager::GameManager()
: _image(imgWidth*scaling, imgHeight*scaling, QImage::Format_ARGB32),
 _cowTexture(":/animals/cow"), _fishTexture(":/animals/fish"), _currentTexture(NULL),
 _sound(this)
{
#ifndef NO_KINECT
    QMediaPlaylist *playlist = new QMediaPlaylist(this);
    playlist->addMedia(QUrl("qrc:/sounds/bells"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    playlist->setCurrentIndex(1);
    _sound.setPlaylist(playlist);
#endif


    srand (time(NULL));
    _playing=false;
    _image.fill(QColor(0,0,0,0));
}

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();

    for(size_t i=0;i<_animals.size();++i)
        delete _animals[i];
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

void GameManager::toggleSetupMode(const bool isSetup, const int minH, const int maxH, const int gameType, const UnitSquareMapping &mapping)
{
    _minH=minH;
    _maxH=maxH;

    _playing=!isSetup;

    _mapping=mapping;

	if (_playing){
		switch (gameType){
		case 0: //swiss terrain
		{
			_currentTexture = &_cowTexture;
			_animals.resize(nAnimals);
			_textureImgWidth = 3;

			for (int i = 0; i < nAnimals; ++i)
				_animals[i] = new Animal(0.4, 0.7, false);

#ifndef NO_KINECT
			//_sound.play();
#endif
			break;
		}
		case 2:  //fishes
		{
			_currentTexture = &_fishTexture;
			_animals.resize(nAnimals);
			_textureImgWidth = 1;

			for (int i = 0; i < nAnimals; ++i)
				_animals[i] = new Animal(0.0, 0.4, false);
			break;
		}
		default:
		{
			_currentTexture = NULL;
		}
		}

		if (_currentTexture)
			_textureImgHeight = _textureImgWidth / _currentTexture->width()*_currentTexture->height();

		_imgOffsetW = _textureImgWidth / 2;
		_imgOffsetH = _textureImgHeight / 2;

		_gameTimer->start(100);
	}
    else{

        for(size_t i=0;i<_animals.size();++i)
            delete _animals[i];
        _animals.clear();
#ifndef NO_KINECT
        _sound.pause();
#endif
        _gameTimer->stop();
        _image.fill(QColor(0, 0, 0, 0));
        updateTexture();   
    }
}

void GameManager::keyPress(const int key)
{ }


void GameManager::mousePress(const int x, const int y)
{ }

void GameManager::mouseMove(const int x, const int y)
{ }

void GameManager::mouseRelease(const int x, const int y)
{ }




void GameManager::initialize()
{
    _gameTimer = new QTimer(this);
    connect(_gameTimer, SIGNAL(timeout()), this, SLOT(updateGame()));

    updateTexture();
}

void GameManager::updateGame()
{
    

    _image.fill(QColor(0,0,0,0));

    if(!_currentTexture) return;
	
    

    QPainter painter;
    painter.begin(&_image);

    /*for (int i = 0; i < 500; ++i)
    {
        for (int j = 0; j < 500; ++j){
            double localH = _mapping.getHeightFromParam(i / 500.0, j / 500.0);

            if (localH < 0) localH = 0;
            if (localH > 1) localH = 1;

            QColor col(int(255 * localH), 255, 255, 100);
            Point2d p = _mapping.fromParameterization(i / 500.0, j / 500.0);
            p.y() = imgHeight - p.y();

            if (p.y() == imgHeight)
                --p.y();
            assert(p.y() >= 0);
            assert(p.y() < imgHeight);

            if(localH>=0.4 && localH<=0.7)
                painter.fillRect(p.x()*scaling, p.y()*scaling, 6, 6, col);

        }
    }// */

    for(size_t i=0;i<_animals.size();++i)
    {
        Animal &a=*_animals[i];
        a.think(_mapping);
        a.update();


        if(a.alive())
        {
            Point2d p=_mapping.fromParameterization(a.position());
            p.y() = imgHeight - p.y();

            Point2d d=_mapping.fromParameterizationDir(a.direction());
            d.y() *= -1;

            const double angle=atan2(d.y(),d.x())*180/PI-90;


            if (p.y() == imgHeight)
                --p.y();
            assert(p.y() >= 0);
            assert(p.y() < imgHeight);

            painter.translate((p.x()+_imgOffsetW)*scaling,(p.y()+_imgOffsetH)*scaling);
            painter.rotate(angle);

            painter.drawImage(QRectF(-_imgOffsetW, -_imgOffsetW,_textureImgWidth*scaling, _textureImgHeight*scaling), *_currentTexture);

            painter.resetTransform();
        }
    }//*/


    painter.end();

    updateTexture();
}
