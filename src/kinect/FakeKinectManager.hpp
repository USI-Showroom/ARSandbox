#ifndef __KINECT_MANAGER_H__
#define __KINECT_MANAGER_H__

#include "IKinectManager.hpp"

class FakeKinectManager : public IKinectManager
{
public:
    FakeKinectManager(IKinectProcessor *processor)
    : IKinectManager(processor)
     { }

    virtual ~FakeKinectManager() { }

    virtual void update() { _processor->processDepth(0, NULL, 500, 500, 0, 10); }
    virtual bool initialize() { return true; }

};
#endif //__KINECT_MANAGER_H__
