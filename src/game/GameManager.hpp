#ifndef __GAME_MANAGER_HPP__
#define __GAME_MANAGER_HPP__

#include "IKinectProcessor.hpp"

#include <QObject>


class QTimer;

class GameManager : public QObject
{
    Q_OBJECT
public:
    GameManager();
    ~GameManager();

private slots:
    void updateGame();

public slots:
    void newKinectData(const UINT16 *data, int w, int h);
    void keyPress(const int key);

public:
    void initialize();

private:
    QTimer *_gameTimer;
};
#endif

