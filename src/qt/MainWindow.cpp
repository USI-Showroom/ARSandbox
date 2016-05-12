#include <iostream>


#include "MainWindow.hpp"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindow)
{
	_ui->setupUi(this);

   _kinectCommunicator.initialize();
   
#ifndef NO_KINECT
  this->showFullScreen();
#endif
  
   connect(&_kinectCommunicator, SIGNAL(newData(const UINT16 *, int , int )),
       _ui->mainView, SLOT(newKinectData(const UINT16 *, int , int )));

   connect(&_kinectCommunicator, SIGNAL(newData(const UINT16 *, int , int )),
     &_gameManager, SLOT(newKinectData(const UINT16 *, int , int )));

   connect(_ui->mainView, SIGNAL(keyPress(const int)),
    &_gameManager, SLOT(keyPress(const int)));

   connect(_ui->mainView, SIGNAL(mousePress(const int, const int, const int, const int)),
    &_gameManager, SLOT(mousePress(const int, const int, const int, const int)));

   connect(_ui->mainView, SIGNAL(mouseMove(const int, const int, const int, const int)),
    &_gameManager, SLOT(mouseMove(const int, const int, const int, const int)));

   connect(_ui->mainView, SIGNAL(mouseRelease(const int, const int, const int, const int)),
    &_gameManager, SLOT(mouseRelease(const int, const int, const int, const int)));


   connect(&_gameManager, SIGNAL(gameTextureChanged(const QImage&)),
    _ui->mainView, SLOT(newGameImage(const QImage&)));

    // here too!
    // connect(&_gameManager, SIGNAL(gameTextureChanged(const QImage&)),
    // _ui->mainView, SLOT(newGameImage(const QImage&)));

   connect(_ui->mainView, SIGNAL(toggleSetupMode(const bool, const int, const int, const int, const UnitSquareMapping &)),
    &_gameManager, SLOT(toggleSetupMode(const bool, const int, const int, const int, const UnitSquareMapping &)));

   _gameManager.initialize();
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    _ui->mainView->keyPressEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
	_ui->mainView->mouseReleaseEvent(e);
}

MainWindow::~MainWindow()
{
	delete _ui;
}


