#include "GameManager.hpp"
#include <QTimer>
#include <iostream>

#include <QColor>
#include <QPainter>
#include <QGLWidget>

#include <time.h>

#include "Point3.hpp"

#define PI 3.1415926535897932384626433832795


static const int imgWidth=512;
static const int imgHeight=424;
static const int scaling=10;

static int nAnimals=10;

GameManager::GameManager()
	: _image(imgWidth*scaling, imgHeight*scaling, QImage::Format_ARGB32), _cowTexture(":/animals/cow")
{
    _sound.setSource(QUrl("qrc:/sounds/bells"));
    _sound.setLoopCount(QSoundEffect::Infinite);
    
    srand (time(NULL));
    _playing=false;
    _image.fill(QColor(0,0,0,0));

    for(int i=0;i<nAnimals;++i)
        _animals.push_back(new Animal(0.5,0.65));
}

GameManager::~GameManager()
{
    if(_gameTimer)
        _gameTimer->stop();

    for(int i=0;i<nAnimals;++i)
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

void GameManager::toggleSetupMode(const bool isSetup, const int minH, const int maxH, const UnitSquareMapping &mapping)
{
    _minH=minH;
    _maxH=maxH;

    _playing=!isSetup;

    _mapping=mapping;

    if(_playing){
        _sound.play();
        _gameTimer->start(100);
    }
    else{
        _sound.stop();
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
    static const double textureImgWidth=10;
	static const double textureImgHeight = textureImgWidth / 134.0*384.0;

	static const double imgOffsetW = textureImgWidth / 2;
	static const double imgOffsetH = textureImgHeight / 2;

    _image.fill(QColor(0, 0, 0, 0));
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

            if(localH>=0.5 && localH<=0.65)
			 painter.fillRect(p.x()*scaling, p.y()*scaling, 6, 6, col);

		}
	}//*/

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

            painter.translate((p.x()+imgOffsetW)*scaling,(p.y()+imgOffsetH)*scaling);
            painter.rotate(angle);

            painter.drawImage(QRectF(-imgOffsetW, -imgOffsetW,textureImgWidth*scaling, textureImgHeight*scaling), _cowTexture);

            painter.resetTransform();
        }
    }//*/


    painter.end();

    updateTexture();
}
