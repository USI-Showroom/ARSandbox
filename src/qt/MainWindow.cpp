#include <iostream>


#include "MainWindow.hpp"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindow)
{
	_ui->setupUi(this);

    _kinectCommunicator.initialize();
    _gameManager.initialize();
	//this->showFullScreen();

	connect(&_kinectCommunicator, SIGNAL(newData(const UINT16 *, int , int )),
		           _ui->mainView, SLOT(newKinectData(const UINT16 *, int , int )));

    connect(&_kinectCommunicator, SIGNAL(newData(const UINT16 *, int , int )),
                   &_gameManager, SLOT(newKinectData(const UINT16 *, int , int )));

    connect(_ui->mainView, SIGNAL(keyPress(const int)),
            &_gameManager, SLOT(keyPress(const int)));
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    _ui->mainView->keyPressEvent(e);
}

MainWindow::~MainWindow()
{
	delete _ui;
}


