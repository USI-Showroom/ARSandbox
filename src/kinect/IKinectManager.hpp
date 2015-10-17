#ifndef __IKINECT_MANAGER_H__
#define __IKINECT_MANAGER_H__

#include "IKinectProcessor.hpp"

class IKinectManager
{
public:
    IKinectManager(IKinectProcessor *processor)
    : _processor(processor)
     { }

    virtual ~IKinectManager() { }

    virtual void update() = 0;
    virtual bool initialize() = 0;

protected:
    IKinectProcessor *_processor;
};
#endif //__IKINECT_MANAGER_H__
