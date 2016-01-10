#define NO_KINECT

#ifndef __IKINECT_PROCESSOR_H__
#define __IKINECT_PROCESSOR_H__

#ifdef __APPLE__
#include <stdint.h>
#define UINT16 uint16_t
#define USHORT int
#define INT64 int
#endif

#ifdef WIN32
#include <windows.h>
#endif

class IKinectProcessor
{
public:
    IKinectProcessor() { }
    virtual ~IKinectProcessor() { }

	virtual void processDepth(INT64 time, const UINT16* buffer, int height, int width, USHORT minDepth, USHORT maxDepth) = 0;
    
};
#endif
