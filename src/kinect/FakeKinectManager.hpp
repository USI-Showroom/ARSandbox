#ifndef __FAKE_KINECT_MANAGER_H__
#define __FAKE_KINECT_MANAGER_H__

#include "IKinectManager.hpp"

class FakeKinectManager : public IKinectManager
{
public:
	FakeKinectManager(IKinectProcessor *processor)
		: IKinectManager(processor)
	{
		_w = 300;
		_h = 150;
		_data = new UINT16[_w*_h];
		_count = 0;

		for (int i = 0; i < _w*_h; ++i)
			_data[i] = i;
	}

    virtual ~FakeKinectManager() { 
        delete[] _data;
    }

    virtual void update() {
        ++_count;
        if(_count>10){
            _count=0;
            _processor->processDepth(0, _data, _w, _h, 0, 10);
        }
    }
    virtual bool initialize() { return true; }

private:
    int _count;
    int _w, _h;
    UINT16 *_data;
};
#endif //__FAKE_KINECT_MANAGER_H__
