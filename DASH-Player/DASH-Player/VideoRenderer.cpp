#include "VideoRenderer.h"
#include <qimage.h>

VideoRenderer::VideoRenderer(Frame *frame, deque<QImage*> *frameBuffer, QMutex *frameBufferMutex, QWaitCondition *frameBufferNotEmpty, QWaitCondition *frameBufferNotFull) :
	frame(frame),
	frameBuffer(frameBuffer),
	frameBufferMutex(frameBufferMutex),
	frameBufferNotEmpty(frameBufferNotEmpty),
	frameBufferNotFull(frameBufferNotFull)
{
}

VideoRenderer::~VideoRenderer()
{
	//this->quit();
	//this->wait();
}

static void print(string string) // for testing
{
	ofstream myfile;
	myfile.open("Log.txt", ios::app);
	myfile << string << endl;
	myfile.close();
}

void VideoRenderer::run()
{
	QImage *image = NULL;
	forever
	{
		frameBufferMutex->lock();
		if (frameBuffer->size() < 1)
		{
			frameBufferNotEmpty->wait(frameBufferMutex);
		}
		frameBufferMutex->unlock();

		image = *frameBuffer->begin();
		frameBuffer->pop_front();
		frame->showFrame(&(QPixmap::fromImage(*image)));
		numberOfRenderedFrames++;
		msleep(1000 / framerate);
		image->~QImage();

		frameBufferMutex->lock();
		if (frameBuffer->size() < frameBufferSize)
		{
			frameBufferNotFull->wakeAll();
		}
		frameBufferMutex->unlock();
	}
}

long VideoRenderer::getNumberOfRenderedFrames()
{
	return numberOfRenderedFrames;
}

void VideoRenderer::setFramerate(int framerate)
{
	this->framerate = framerate;
}
