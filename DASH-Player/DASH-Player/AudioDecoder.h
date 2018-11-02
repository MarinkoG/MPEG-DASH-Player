#pragma once
#include "Decoder.h"
#include <libavutil\frame.h>
#include <qaudioformat.h>
#include "AudioSample.h"

class AudioDecoder : public Decoder
{
	Q_OBJECT

public:
	AudioDecoder(deque<ISegment*> *segmentBuffer, QMutex *segmentBufferMutex, QWaitCondition *segmentBufferNotEmpty, deque<AudioSample*> *audioSampleBuffer, QMutex *audioSampleBufferMutex, QWaitCondition *audioSampleBufferNotEmpty, QWaitCondition *audioSampleBufferNotFull);
	~AudioDecoder();
	void upis(const uint8_t ** data, long l);
	void saveToBuffer(AVCodecContext *codecContext, AVFrame *frame)override;

private:
	deque<AudioSample*> *audioSampleBuffer;
	QMutex * audioSampleBufferMutex;
	QWaitCondition *audioSampleBufferNotEmpty;
	QWaitCondition *audioSampleBufferNotFull;
	int audioSampleBufferSize = 4000;
	AudioSample *audioSample;

signals:
	void framesDecoded(QAudioFormat *format);

};
