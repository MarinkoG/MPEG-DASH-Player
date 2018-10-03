#pragma once
#include <qobject.h>
#include <qthread.h>
#include <deque>
#include <ISegment.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qimage.h>
#include "MediaEngine.h"
extern "C"
{
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#include<libavutil\imgutils.h>
}


using namespace std;
using namespace dash::mpd;

class SegmentDecoder : public QThread
{
	Q_OBJECT

public:
	SegmentDecoder(QMutex* frameBufferLock, deque<QImage*>* frameBuffer, QMutex* segmentBufferLock, deque<ISegment*>* segmentBuffer);
	~SegmentDecoder();
	void run() override;

private:
	QMutex * frameBufferLock;
	deque<QImage*>* frameBuffer;
	deque<ISegment*>* segmentBuffer;
	QMutex* segmentBufferLock;

signals:
	void decodingFinished(const QString &result);
};
