#pragma once
#include <qthread.h>
#include "Frame.h"
#include <deque>
#include <qimage.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

class VideoRenderer : public QThread
{
	Q_OBJECT

public:
	VideoRenderer(Frame *frame, deque<QImage*> *frameBuffer, QMutex *frameBufferMutex, QWaitCondition *frameBufferNotEmpty, QWaitCondition *frameBufferNotFull);
	~VideoRenderer();
	void run() override;
	void setFramerate(double framerate);
	long getNumberOfRenderedFrames();
	int frameBufferSize = 500;

private:
	Frame *frame;
	double framerate = 24;
	deque<QImage*> *frameBuffer;
	QMutex *frameBufferMutex;
	QWaitCondition *frameBufferNotEmpty;
	QWaitCondition *frameBufferNotFull;
	long numberOfRenderedFrames = 0;

};

