#pragma once
#include "Decoder.h"

class VideoDecoder : public Decoder
{
	Q_OBJECT

public:
	VideoDecoder(deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty, deque<QImage*> *frameBuffer, QMutex *frameBufferMutex, QWaitCondition *frameBufferNotEmpty, QWaitCondition *frameBufferNotFull);
	~VideoDecoder();
	void saveToBuffer(AVCodecContext *codecContext, AVFrame *frame)override;

private:
	deque<QImage*> *frameBuffer;
	QMutex * frameBufferMutex;
	QWaitCondition *frameBufferNotEmpty;
	QWaitCondition *frameBufferNotFull;
	int frameBufferSize = 500;

signals:
	void framesDecoded(double framerate);

};
