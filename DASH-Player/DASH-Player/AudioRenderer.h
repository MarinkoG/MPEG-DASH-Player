#pragma once
#include <qthread.h>

extern "C"
{
#include <libavutil\frame.h>
}
#include <string.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <deque>
#include <qaudioformat.h>
#include <qaudiooutput.h>
#include <qiodevice.h>
#include <qaudiodeviceinfo.h>
#include "AudioSample.h"

#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

class AudioRenderer : public QThread
{
	Q_OBJECT

public:
	AudioRenderer(deque<AudioSample*> *audioSampleBuffer, QMutex *audioSampleBufferMutex, QWaitCondition *audioSampleBufferNotEmpty, QWaitCondition *audioSampleBufferNotFull);
	~AudioRenderer();
	void run() override;
	void setFramerate(double framerate);
	long getNumberOfRenderedFrames();
	void setAudioFormat(QAudioFormat *format);

	void WriteToBuffer(const char * data, qint64 len);

private:

	deque<AudioSample*> *audioSampleBuffer;
	QMutex * audioSampleBufferMutex;
	QWaitCondition *audioSampleBufferNotEmpty;
	QWaitCondition *audioSampleBufferNotFull;
	int audioSampleBufferSize = 4000;
	QAudioFormat *format;

	QAudioOutput        *audioOutput;
	QAudioDeviceInfo    deviceInfo;
	QIODevice           *output;

	void    init();
};
