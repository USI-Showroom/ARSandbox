#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include <vector>
#include <QMainWindow>

#include "KinectComunicator.hpp"
#include "GameManager.hpp"

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::MainWindow* _ui;
    KinectComunicator _kinectCommunicator;
    GameManager _gameManager;
};



#endif // __MAINWINDOW_HPP__
