#ifdef WIN32
#ifndef __KINECT_MANAGER_H__
#define __KINECT_MANAGER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <Kinect.h>

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
	template<class Interface>
	static inline void SafeRelease(Interface *& interfaceToRelease)
	{
		if (interfaceToRelease != NULL)
		{
			interfaceToRelease->Release();
			interfaceToRelease = NULL;
		}
	}

    // Current Kinect
    IKinectSensor*          _kinectSensor;

    // Depth reader
    IDepthFrameReader*      _depthFrameReader;
};
#endif //__KINECT_MANAGER_H__
#endif
