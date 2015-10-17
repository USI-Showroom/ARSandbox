#include <iostream>


#include "MainWindow.hpp"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindow)
{
	_ui->setupUi(this);

    _kinectCommunicator.initialize();
}

MainWindow::~MainWindow()
{
	delete _ui;
}


