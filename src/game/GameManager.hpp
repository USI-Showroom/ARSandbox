#ifndef __GAME_MANAGER_HPP__
#define __GAME_MANAGER_HPP__

#include "IKinectProcessor.hpp"

#include <QObject>
#include <QImage>
#include <vector>

#include "Point2.hpp"
#include "UnitSquareMapping.hpp"
#include "Drop.hpp"
#include "Simulation.hpp"
#include "Grid.hpp"

class QTimer;

class GameManager : public QObject
{
    Q_OBJECT
public:
    GameManager();
    ~GameManager();

signals:
    void gameTextureChanged(const QImage &img);
    void rangeChanged(const float h);

private slots:
    void updateGame();

public slots:
    void newKinectData(const UINT16 *data, int w, int h);
    
    void toggleSetupMode(const bool isSetup, const int minH, const int maxH, const int gameType, const UnitSquareMapping &mapping);
    
    void keyPress(const int key);
    void mousePress(const int x, const int y, const int w, const int h);
    void mouseMove(const int x, const int y,  const int w, const int h);
    void mouseRelease(const int x, const int y, const int w, const int h);
    

public:
    void initialize();
    void updateTexture();

private:
    std::vector<Drop *> _drops;
    int _minH, _maxH;
    
    bool _playing;
    UnitSquareMapping _mapping;

    QTimer *_gameTimer;
    QImage _image;

    Simulation *_simulation;
    Grid *_grid;
};
#endif

