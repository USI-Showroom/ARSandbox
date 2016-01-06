#ifndef __GAME_MANAGER_HPP__
#define __GAME_MANAGER_HPP__

#include "IKinectProcessor.hpp"

#include <QObject>
#include <QImage>

#include "Point2.hpp"


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
    void toggleSetupMode(const bool isSetup, const int minH, const int maxH);

public:
    void initialize();
    void updateTexture();

private:
    double getHeight(const UINT16 *data, const int x, const int y);

private:
    int _minH, _maxH;
    QTimer *_gameTimer;
    QImage _image;
    bool _playing;


    Point2d _position;
};
#endif

