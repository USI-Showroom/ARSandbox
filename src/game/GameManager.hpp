#ifndef __GAME_MANAGER_HPP__
#define __GAME_MANAGER_HPP__

#include "IKinectProcessor.hpp"

#include <QObject>
#include <QImage>
#include <QMediaPlayer>
#include <vector>

#include "Point2.hpp"
#include "UnitSquareMapping.hpp"
#include "Animal.hpp"

class QTimer;

class GameManager : public QObject
{
    Q_OBJECT
public:
    GameManager();
    ~GameManager();

signals:
    void gameTextureChanged(const QImage &img);

private slots:
    void updateGame();

public slots:
    void newKinectData(const UINT16 *data, int w, int h);
    void keyPress(const int key);
    void toggleSetupMode(const bool isSetup, const int minH, const int maxH, const UnitSquareMapping &mapping);

public:
    void initialize();
    void updateTexture();

private:
    int _minH, _maxH;
    QTimer *_gameTimer;
    QImage _image;
    bool _playing;
    UnitSquareMapping _mapping;
	QMediaPlayer _sound;

    std::vector<Animal *> _animals;

    QImage _cowTexture;
};
#endif

