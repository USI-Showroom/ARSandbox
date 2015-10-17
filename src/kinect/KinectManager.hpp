#ifndef WIN32
#ifndef __KINECT_MANAGER_H__
#define __KINECT_MANAGER_H__

#include "IKinectManager.hpp"

class KinectManager : public IKinectManager
{
    static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;

public:
    KinectManager(IKinectProcessor *processor);
    ~KinectManager();

    void update();
    bool initialize();

private:
    // Current Kinect
    IKinectSensor*          _kinectSensor;

    // Depth reader
    IDepthFrameReader*      _depthFrameReader;
};
#endif //__KINECT_MANAGER_H__
#endif
