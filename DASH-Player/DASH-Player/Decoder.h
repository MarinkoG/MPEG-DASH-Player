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
using namespace dash::network;

class Decoder : public QThread
{
	Q_OBJECT

public:
	Decoder(deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty);
	~Decoder();
	void print(string string);
	void run() override;
	virtual void saveToBuffer(AVCodecContext *codecContext, AVFrame *frame);
	void setMediaType(AVMediaType mediaType);
	long numberOfFrames = 0;

private:
	size_t bufferSize = 32768;
	deque<ISegment*> *segmentBuffer;
	QMutex *segmentBufferMutex;
	QWaitCondition *segmentBufferNotEmpty;
	int width;
	int height;
	void decode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *pkt);
	//long numberOfFrames = 0;
	int stream = -1;
	AVMediaType mediaType = AVMEDIA_TYPE_VIDEO;

signals:
	void segmentDecoded(long numberOfFrames);
	
};
