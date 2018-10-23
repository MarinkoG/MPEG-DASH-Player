#pragma once
#include <qobject.h>
#include <qthread.h>
#include <deque>
#include <ISegment.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qimage.h>
extern "C"
{
	#include <libavcodec\avcodec.h>
	#include <libavformat\avformat.h>
	#include <libswscale\swscale.h>
	#include<libavutil\imgutils.h>
}
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;
using namespace dash::mpd;

class SegmentDecoder : public QThread
{
	Q_OBJECT

public:
	SegmentDecoder(deque<QImage*> *frameBuffer, QMutex *frameBufferMutex, QWaitCondition *frameBufferNotEmpty, deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty);
	~SegmentDecoder();
	void run() override;

private:
	size_t bufferSize = 32768;
	deque<QImage*> *frameBuffer;
	deque<ISegment*> *segmentBuffer;
	QMutex *frameBufferMutex;
	QWaitCondition *frameBufferNotEmpty;
	QMutex *segmentBufferMutex;
	QWaitCondition *segmentBufferNotEmpty;
	int width;
	int height;
	void decode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *pkt);
	void saveToBuffer(AVFrame *rgbFrame);
	long numberOfFrames = 0;

signals:
	void segmentDecoded(long numberOfFrames);
};
