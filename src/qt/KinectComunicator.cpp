#include "KinectComunicator.hpp"
#include <QTimer>
#include <iostream>

#include "IKinectManager.hpp"

#ifdef WIN32
#include "KinectManager.hpp"
#endif
#include "FakeKinectManager.hpp"


KinectComunicator::KinectComunicator()
: _manager(NULL), _kinectTimer(NULL)
{
#ifdef WIN32
#ifndef NO_KINECT
    _manager=new KinectManager(this);
#else
	_manager=new FakeKinectManager(this);
#endif
#else
    _manager=new FakeKinectManager(this);
#endif
}

KinectComunicator::~KinectComunicator()
{
    if(_kinectTimer)
        _kinectTimer->stop();

    delete _kinectTimer;
    delete _manager;
}


void KinectComunicator::initialize()
{
    _manager->initialize();

    _kinectTimer = new QTimer(this);
    connect(_kinectTimer, SIGNAL(timeout()), this, SLOT(updateKinectManager()));
    _kinectTimer->start(40);
}

void KinectComunicator::updateKinectManager()
{
    _manager->update();
}

void KinectComunicator::processDepth(INT64 time, const UINT16* buffer, int width, int height, USHORT minDepth, USHORT maxDepth)
{
    emit newData(buffer,width,height);
}