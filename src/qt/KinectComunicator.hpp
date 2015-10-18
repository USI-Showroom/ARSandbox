#ifndef __KINECT_COMUNICATOR_H__
#define __KINECT_COMUNICATOR_H__

#include <QObject>

#include "IKinectProcessor.hpp"

class IKinectManager;
class QTimer;

class KinectComunicator : public QObject, public IKinectProcessor
{
    Q_OBJECT
public:
    KinectComunicator();
    ~KinectComunicator();

    void initialize();

    virtual void processDepth(INT64 time, const UINT16* buffer, int height, int width, USHORT minDepth, USHORT maxDepth);

    private slots:
    void updateKinectManager();

signals:
    void newData(const UINT16 *buffer);
private:
    IKinectManager *_manager;
    QTimer *_kinectTimer;
};
#endif

