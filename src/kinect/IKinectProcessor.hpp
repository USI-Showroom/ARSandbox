#ifndef __IKINECT_PROCESSOR_H__
#define __IKINECT_PROCESSOR_H__

#ifdef __APPLE__
#include <stdint.h>
#define UINT16 uint16_t
#define USHORT int
#endif

class IKinectProcessor
{
public:
    IKinectProcessor() { }
    virtual ~IKinectProcessor() { }

    virtual void processDepth(int time, const UINT16* buffer, int height, int width, USHORT minDepth, USHORT maxDepth) = 0;
    
};
#endif
