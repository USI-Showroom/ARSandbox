#ifdef WIN32
#include "KinectManager.hpp"


KinectManager::KinectManager(IKinectProcessor *processor)
: IKinectManager(processor), _kinectSensor(NULL), _depthFrameReader(NULL),
{ }

KinectManager::~KinectManager()
{
    SafeRelease(_depthFrameReader);
    if (_kinectSensor)
        _kinectSensor->Close();

    SafeRelease(_kinectSensor);
}


void KinectManager::update()
{
    if (!_depthFrameReader)
    {
        return;
    }

    IDepthFrame* depthFrame = NULL;

    HRESULT hr = _depthFrameReader->AcquireLatestFrame(&depthFrame);

    if (SUCCEEDED(hr))
    {
        int time = 0;
        IFrameDescription* frameDescription = NULL;
        int width = 0;
        int height = 0;
        USHORT depthMinReliableDistance = 0;
        USHORT depthMaxDistance = 0;
        UINT bufferSize = 0;
        UINT16 *buffer = NULL;

        hr = depthFrame->get_RelativeTime(&time);

        if (SUCCEEDED(hr))
            hr = depthFrame->get_FrameDescription(&frameDescription);

        if (SUCCEEDED(hr))
            hr = frameDescription->get_Width(&width);

        if (SUCCEEDED(hr))
            hr = frameDescription->get_Height(&height);
        
        if (SUCCEEDED(hr))
            hr = depthFrame->get_DepthMinReliableDistance(&depthMinReliableDistance);

        if (SUCCEEDED(hr))
        {
            depthMaxDistance = USHRT_MAX;
            hr = depthFrame->get_DepthMaxReliableDistance(&depthMaxDistance);
        }

        if (SUCCEEDED(hr)) hr = depthFrame->AccessUnderlyingBuffer(&bufferSize, &buffer);            
        
        if (SUCCEEDED(hr))
            processor->processDepth(time, buffer, width, height, depthMinReliableDistance, depthMaxDistance);

        SafeRelease(frameDescription);
    }

    SafeRelease(depthFrame);
}

bool KinectManager::initialize()
{
    HRESULT hr;

    hr = GetDefaultKinectSensor(&_kinectSensor);
    if (FAILED(hr))
        return false;
    

    if (_kinectSensor)
    {
        IDepthFrameSource* depthFrameSource = NULL;

        hr = _kinectSensor->Open();

        if (SUCCEEDED(hr))
            hr = _kinectSensor->get_DepthFrameSource(&depthFrameSource);

        if (SUCCEEDED(hr))
            hr = depthFrameSource->OpenReader(&_depthFrameReader);

        SafeRelease(depthFrameSource);
    }

    if (!_kinectSensor || FAILED(hr))
    {
        std::cerr<<"No ready Kinect found!"<<std::endl;
        return false;
    }

    return true;
}

#endif