#ifndef __FAKE_KINECT_MANAGER_H__
#define __FAKE_KINECT_MANAGER_H__

#include "IKinectManager.hpp"

#include <QImage>

class FakeKinectManager : public IKinectManager
{
public:
    FakeKinectManager(IKinectProcessor *processor)
    : IKinectManager(processor)
    {

        QImage tmp(":/test/depth");

        _w = tmp.width();
        _h = tmp.height();
        _data = new UINT16[_w*_h];
        _count = 0;

        int index=0;

        for(int j=0;j<_h;++j)
        {
            for (int i = 0; i < _w; ++i)
            {
                _data[index++] = qRed(tmp.pixel(i,j));
            }
        }
    }

    virtual ~FakeKinectManager() { 
        delete[] _data;
    }

    virtual void update() {
        ++_count;
        if(_count>10){
            _count=0;
            _processor->processDepth(0, _data, _w, _h, 0, 255);
        }
    }
    virtual bool initialize() { return true; }

private:
    int _count;
    int _w, _h;
    UINT16 *_data;
};
#endif //__FAKE_KINECT_MANAGER_H__
